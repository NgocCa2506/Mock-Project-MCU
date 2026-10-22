/*
 * boot_ota.c
 *
 *  OTA update: copy the OTA image (Sector 9-11) over the App (Sector 5-7).
 *
 *  Flow (called by the Bootloader when .ota_flag == 1):
 *    1. Clear .ota_flag
 *    2. Copy binary  : OTA code -> App code   (method selected by OTA_METHOD)
 *    3. Copy header  : OTA header -> App header, version = old version + 1
 *  The caller then continues the normal jump flow (CRC and header check).
 */

#include "main.h"
#include "boot_ota.h"
#include "app_header.h"
#include "flash_layout.h"
#include "crc32.h"
#include "crc_benchmark.h"
#include "led.h"

#define OTA_BLINK_START_TOGGLES     6U      /* 3 orange blinks before OTA   */
#define OTA_BLINK_WORDS             256U    /* toggle orange every 1 KB     */

volatile uint32_t g_ota_cycles_overwrite   = 0U;
volatile uint32_t g_ota_cycles_erase_write = 0U;

static const uint32_t s_magic[APP_MAGIC_WORD_COUNT] =
{
    APP_MAGIC_WORD_0,
    APP_MAGIC_WORD_1,
    APP_MAGIC_WORD_2,
    APP_MAGIC_WORD_3,
    APP_MAGIC_WORD_4
};

/* Erase Sector 4, then write the whole header from RAM. */
static void header_write(const app_header_t *header)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t sector_error;
    const uint32_t *word = (const uint32_t *)header;
    uint32_t i;

    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = FLASH_SECTOR_4;
    erase.NbSectors    = 1U;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASH_Unlock();

    if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK)
    {
        Error_Handler();
    }

    for (i = 0U; i < (sizeof(app_header_t) / 4U); i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                              APP_HEADER_START_ADDRESS + (i * 4U),
                              word[i]) != HAL_OK)
        {
            Error_Handler();
        }
    }

    HAL_FLASH_Lock();
}

/* Do not touch the App if the OTA image is not valid. */
static uint8_t ota_image_valid(const app_header_t *ota_header)
{
    uint32_t i;

    for (i = 0U; i < APP_MAGIC_WORD_COUNT; i++)
    {
        if (ota_header->magic_number[i] != s_magic[i])
        {
            return 0U;
        }
    }

    if ((ota_header->app_size == 0U) ||
        (ota_header->app_size > APP_SIZE) ||
        (ota_header->app_size > OTA_SIZE))
    {
        return 0U;
    }

    if (crc32_calculate((const uint8_t *)OTA_START_ADDRESS,
                        ota_header->app_size) != ota_header->app_crc)
    {
        return 0U;
    }

    return 1U;
}

/* Write the OTA binary word by word over the App code. Flash must be unlocked. */
static void ota_copy_binary(uint32_t size)
{
    uint32_t words = (size + 3U) / 4U;
    uint32_t i;

    for (i = 0U; i < words; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                              APP_START_ADDRESS + (i * 4U),
                              *(volatile uint32_t *)(OTA_START_ADDRESS + (i * 4U))) != HAL_OK)
        {
            Error_Handler();
        }

        if ((i % OTA_BLINK_WORDS) == 0U)
        {
            led_orange_toggle();
        }
    }
}

void boot_ota_run(void)
{
    const app_header_t *app_header = (const app_header_t *)APP_HEADER_START_ADDRESS;
    const app_header_t *ota_header = (const app_header_t *)OTA_HEADER_START_ADDRESS;
    app_header_t new_header;
    uint32_t old_version;
    uint32_t start_cycle;
    uint32_t i;

    crc_benchmark_dwt_init();

    /* OTA check: blink orange LED */
    for (i = 0U; i < OTA_BLINK_START_TOGGLES; i++)
    {
        led_orange_toggle();
        HAL_Delay(LED_BLINK_PERIOD_MS);
    }

    /* 1. Clear .ota_flag, keep the rest of the App header */
    new_header  = *app_header;
    old_version = new_header.version;
    new_header.ota_flag = 0U;
    header_write(&new_header);

    if (ota_image_valid(ota_header) == 0U)
    {
        return;     /* nothing copied, App is unchanged */
    }

    /* 2. Copy binary from OTA code to App code */
    new_header = *ota_header;       /* RAM copy, used in step 3 */

    HAL_FLASH_Unlock();
    start_cycle = DWT->CYCCNT;

#if (OTA_METHOD == OTA_METHOD_OVERWRITE)
    /* Method 1: write over the App code, no erase */
    ota_copy_binary(new_header.app_size);
    g_ota_cycles_overwrite = DWT->CYCCNT - start_cycle;

#elif (OTA_METHOD == OTA_METHOD_ERASE_FIRST)
    /* Method 2: erase App sectors 5-7, then write */
    {
        FLASH_EraseInitTypeDef erase;
        uint32_t sector_error;

        erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
        erase.Sector       = FLASH_SECTOR_5;
        erase.NbSectors    = 3U;
        erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

        if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK)
        {
            Error_Handler();
        }
    }
    ota_copy_binary(new_header.app_size);
    g_ota_cycles_erase_write = DWT->CYCCNT - start_cycle;

#else
#error "Invalid OTA_METHOD"
#endif

    HAL_FLASH_Lock();

    /* 3. Copy header from OTA header to App header, version + 1 */
    new_header.version  = old_version + 1U;
    new_header.ota_flag = 0U;
    header_write(&new_header);
}

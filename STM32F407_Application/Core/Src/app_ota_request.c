/*
 * app_ota_request.c
 *
 *  Ask the Bootloader for an OTA update.
 */

#include "main.h"
#include "app_ota_request.h"
#include "app_header.h"
#include "flash_layout.h"

/*
 * Flash voltage range for erase and program operations, select exactly one:
 *
 *   FLASH_PROGRAM_VOLTAGE_RANGE_3  (2.7 V – 3.6 V)
 *     Erase  : FLASH_VOLTAGE_RANGE_3   (x32 parallelism)
 *     Program: FLASH_TYPEPROGRAM_WORD  (4 bytes per write)
 *
 *   FLASH_PROGRAM_VOLTAGE_RANGE_4  (2.7 V – 3.6 V + external Vpp 8–9 V)
 *     Erase  : FLASH_VOLTAGE_RANGE_4   (x64 parallelism)
 *     Program: FLASH_TYPEPROGRAM_DOUBLEWORD (8 bytes per write)
 *
 * Must match the setting used in the Bootloader (boot_ota.h).
 */
#define FLASH_PROGRAM_VOLTAGE_RANGE_3   3U
#define FLASH_PROGRAM_VOLTAGE_RANGE_4   4U

#define FLASH_PROGRAM_VOLTAGE           FLASH_PROGRAM_VOLTAGE_RANGE_3

void app_ota_request(void)
{
    app_header_t header;
    FLASH_EraseInitTypeDef erase;
    uint32_t sector_error;
    uint32_t i;

    /* Copy the current header (read from Flash) to RAM and set the flag. */
    header = *(const volatile app_header_t *)APP_HEADER_START_ADDRESS;
    header.ota_flag = 1U;

    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = FLASH_SECTOR_4;
    erase.NbSectors    = 1U;

#if (FLASH_PROGRAM_VOLTAGE == FLASH_PROGRAM_VOLTAGE_RANGE_4)
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_4;
#else
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#endif

    HAL_FLASH_Unlock();

    /* Flash can only be rewritten after erase: erase Sector 4, write header. */
    if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return;
    }

#if (FLASH_PROGRAM_VOLTAGE == FLASH_PROGRAM_VOLTAGE_RANGE_4)
    /* Range 4: program 8 bytes (doubleword) at a time. */
    {
        const uint64_t *dword = (const uint64_t *)&header;
        uint32_t count = (sizeof(app_header_t) + 7U) / 8U;

        for (i = 0U; i < count; i++)
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                  APP_HEADER_START_ADDRESS + (i * 8U),
                                  dword[i]) != HAL_OK)
            {
                HAL_FLASH_Lock();
                return;
            }
        }
    }
#else
    /* Range 3: program 4 bytes (word) at a time. */
    {
        const uint32_t *word = (const uint32_t *)&header;

        for (i = 0U; i < (sizeof(app_header_t) / 4U); i++)
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                  APP_HEADER_START_ADDRESS + (i * 4U),
                                  word[i]) != HAL_OK)
            {
                HAL_FLASH_Lock();
                return;
            }
        }
    }
#endif

    HAL_FLASH_Lock();

    NVIC_SystemReset();
}

/*
 * app_ota_request.c
 *
 *  Ask the Bootloader for an OTA update.
 */

#include "main.h"
#include "app_ota_request.h"
#include "app_header.h"
#include "flash_layout.h"

void app_ota_request(void)
{
    app_header_t header;
    FLASH_EraseInitTypeDef erase;
    uint32_t sector_error;
    const uint32_t *word = (const uint32_t *)&header;
    uint32_t i;

    /* Copy the current header (read from Flash) to RAM and set the flag. */
    header = *(const volatile app_header_t *)APP_HEADER_START_ADDRESS;
    header.ota_flag = 1U;

    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = FLASH_SECTOR_4;
    erase.NbSectors    = 1U;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASH_Unlock();

    /* Flash can only be rewritten after erase: erase Sector 4, write header. */
    if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return;
    }

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

    HAL_FLASH_Lock();

    NVIC_SystemReset();
}

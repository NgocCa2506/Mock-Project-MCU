/*
 * app_validate.c
 *
 *  Created on: Oct 12, 2026
 *      Author: CA
 */

#include <stdint.h>

#include "app_validate.h"
#include "flash_layout.h"

/* STM32F407VG SRAM1 + SRAM2 */
#define SRAM_START_ADDRESS          0x20000000UL
#define SRAM_TOP_ADDRESS            0x20020000UL

/* STM32F407VG 64 KB CCM RAM */
#define CCM_START_ADDRESS           0x10000000UL
#define CCM_TOP_ADDRESS             0x10010000UL

#define RESET_HANDLER_OFFSET        4UL
#define THUMB_BIT_MASK              0x00000001UL
#define CLEAR_THUMB_BIT_MASK        0xFFFFFFFEUL

app_status_t validate_app(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;
    uint32_t resetHandlerAddress;
    uint8_t stackInSram;
    uint8_t stackInCcm;

    /* Vector table word 0: Initial Main Stack Pointer. */
    appStack = *(volatile const uint32_t *)APP_START_ADDRESS;

    /* Vector table word 1: Reset Handler address. */
    appResetHandler =
        *(volatile const uint32_t *)(APP_START_ADDRESS + RESET_HANDLER_OFFSET);

    stackInSram =
        ((appStack >= SRAM_START_ADDRESS) &&
         (appStack <= SRAM_TOP_ADDRESS)) ? 1U : 0U;

    stackInCcm =
        ((appStack >= CCM_START_ADDRESS) &&
         (appStack <= CCM_TOP_ADDRESS)) ? 1U : 0U;

    if ((stackInSram == 0U) && (stackInCcm == 0U))
    {
        return APP_INVALID;
    }

    /* MSP must be word aligned. */
    if ((appStack & 0x3UL) != 0UL)
    {
        return APP_INVALID;
    }

    /* Cortex-M Reset Handler address must contain the Thumb bit. */
    if ((appResetHandler & THUMB_BIT_MASK) == 0UL)
    {
        return APP_INVALID;
    }

    resetHandlerAddress = appResetHandler & CLEAR_THUMB_BIT_MASK;

    /* Reset Handler must be located inside the Application region. */
    if ((resetHandlerAddress < APP_START_ADDRESS) ||
        (resetHandlerAddress > APP_END_ADDRESS))
    {
        return APP_INVALID;
    }

    return APP_VALID;
}


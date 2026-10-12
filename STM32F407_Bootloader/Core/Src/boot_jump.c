/*
 * boot_jump.c
 *
 *  Created on: Oct 12, 2026
 *      Author: CA
 */

#include <stdint.h>

#include "main.h"
#include "boot_jump.h"
#include "flash_layout.h"

typedef void (*pFunction)(void);

void jump_to_app(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;

    appStack = *(volatile uint32_t *)APP_START_ADDRESS;
    appResetHandler =
        *(volatile uint32_t *)(APP_START_ADDRESS + 4UL);

    /* Khoa ngat */
    __disable_irq();

    /* Dung SysTick cua Bootloader */
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;

    /* Chuyen vector table sang Application */
    SCB->VTOR = APP_START_ADDRESS;
    __DSB();
    __ISB();

    /*
     * Goi Reset_Handler cua Application.
     * Luu y: MSP chua duoc chuyen sang appStack.
     */
    __enable_irq();

    ((void (*)(void))appResetHandler)();

    while (1)
    {
    }
}

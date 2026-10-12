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
    pFunction appEntry;

    /* Read Application initial Main Stack Pointer. */
    appStack = *(volatile uint32_t *)APP_START_ADDRESS;

    /* Read Application Reset Handler address. */
    appResetHandler = *(volatile uint32_t *)(APP_START_ADDRESS + 4UL);
    appEntry = (pFunction)appResetHandler;

    /* Disable interrupts. */
    __disable_irq();

    /* Stop SysTick. */
    SysTick->CTRL = 0UL;
    SysTick->LOAD = 0UL;
    SysTick->VAL  = 0UL;

    /* Set Application Main Stack Pointer. */
    __set_MSP(appStack);

    /* Jump to Application Reset Handler. */
    appEntry();
}

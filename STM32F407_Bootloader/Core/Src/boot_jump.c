/*
 * boot_jump.c
 *
 *  Created on: Oct 12, 2026
 *      Author: CA
 */

#include <stdint.h>

#include "main.h"
#include "boot_jump.h"
#include <flash_layout.h>

typedef void (*pFunction)(void);

int validate_app(void)
{
    /* Reset handler sanity */
    uint32_t reset_handler = *(uint32_t *)(FLASH_START_ADDRESS + RESET_HANDLER_OFFSET);
    if ((reset_handler & 0xFF000000) != FLASH_START_ADDRESS)
    {
        return APP_RESET_HANDLER_INVALID;
    }

    /* Others check here */

    /* Application is valid */
    return APP_VALID;
}

void jump_to_app(void)
{
    /* The actual application data starts immediately after the header */
    uint32_t APP_ADDR = APP_START_ADDRESS;

    /* Disable interrupts */
    __disable_irq();

    /* Reset the Clock configuration to default HSI */
    HAL_RCC_DeInit();

    /* Stop SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Set main stack pointer (MSP) to the application's stack pointer.
     * It is located at the 1st 32-bit word of the application's vector table */
    uint32_t app_msp = *(__IO uint32_t*)APP_ADDR;
    __set_MSP(app_msp);

    /* Jump to application reset handler.
     * It is located at the 2nd 32-bit word of the application's vector table */
    void (*app_reset_handler)(void) = (void (*)(void))(*(__IO uint32_t*)(APP_ADDR + RESET_HANDLER_OFFSET));
    app_reset_handler();
}

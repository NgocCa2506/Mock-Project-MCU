/*
 * boot_jump.c
 *
 *  Hand over control from the Bootloader to the Application.
 */

#include <stdint.h>

#include "main.h"
#include "boot_jump.h"
#include "flash_layout.h"

typedef void (*pFunction)(void);

void jump_to_app(void)
{
    uint32_t APP_ADDR = APP_START_ADDRESS;

    __disable_irq();

    HAL_RCC_DeInit();

    SysTick->CTRL = 0UL;
    SysTick->LOAD = 0UL;
    SysTick->VAL  = 0UL;

    uint32_t app_msp = *(__IO uint32_t *)APP_ADDR;
    __set_MSP(app_msp);

    void (*app_reset_handler)(void) =
        (void (*)(void))(*(__IO uint32_t *)
        (APP_ADDR + RESET_HANDLER_OFFSET));

    app_reset_handler();
}

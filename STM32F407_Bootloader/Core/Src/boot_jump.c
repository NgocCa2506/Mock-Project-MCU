#include <stdint.h>

#include "main.h"
#include "boot_jump.h"
#include "flash_layout.h"
#include "app_header.h"

typedef void (*pFunction)(void);

int validate_app(void)
{
    app_header_status_t header_status;
    uint32_t reset_handler;

    /* Check Magic Number[5], App Size and App CRC first. */
    header_status = app_header_validate();

    if (header_status == APP_HEADER_MAGIC_INVALID)
    {
        return APP_MAGIC_NUMBER_INVALID;
    }

    if (header_status == APP_HEADER_SIZE_INVALID)
    {
        return APP_SIZE_INVALID;
    }

    if (header_status == APP_HEADER_CRC_INVALID)
    {
        return APP_CRC_INVALID;
    }

    /* Reset Handler is the second word of the Application vector table. */
    reset_handler = *(volatile const uint32_t *)
        (APP_START_ADDRESS + RESET_HANDLER_OFFSET);

    /* Keep the current Flash sanity check. */
    if ((reset_handler & 0xFF000000UL) != FLASH_START_ADDRESS)
    {
        return APP_RESET_HANDLER_INVALID;
    }

    return APP_VALID;
}

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

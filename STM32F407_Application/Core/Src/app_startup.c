/*
 * app_startup.c
 *
 *  Application start-up steps required when it is launched by the Bootloader.
 */

#include "main.h"
#include "app_startup.h"
#include "flash_layout.h"

void app_startup_init(void)
{
    /* Relocate vector table offset to start address of Application */
    SCB->VTOR = APP_START_ADDRESS;

    /* Enable interrupts */
    __enable_irq();
}

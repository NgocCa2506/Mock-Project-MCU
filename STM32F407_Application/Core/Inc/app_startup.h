/*
 * app_startup.h
 *
 *  Application start-up steps required when it is launched by the Bootloader.
 */

#ifndef INC_APP_STARTUP_H_
#define INC_APP_STARTUP_H_

/*
 * Relocate the vector table to APP_START_ADDRESS and enable interrupts.
 * Call this first in main(), before HAL_Init().
 */
void app_startup_init(void);

#endif /* INC_APP_STARTUP_H_ */

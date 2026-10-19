/*
 * led.h
 *
 *  Status LED. In the Bootloader it blinks when no valid Application
 *  is available to start.
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

#define LED_PORT                GPIOD
#define LED_PIN                 GPIO_PIN_14
#define LED_BLINK_PERIOD_MS     100U

void led_toggle(void);

#endif /* INC_LED_H_ */

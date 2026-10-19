/*
 * led.h
 *
 *  Status LED. In the Application it blinks while the Application is running.
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

#define LED_PORT                GPIOD
#define LED_PIN                 GPIO_PIN_15
#define LED_BLINK_PERIOD_MS     100U

void led_toggle(void);

#endif /* INC_LED_H_ */

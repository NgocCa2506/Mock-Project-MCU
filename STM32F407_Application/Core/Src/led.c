/*
 * led.c
 *
 *  Status LED.
 */

#include "led.h"

void led_toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}

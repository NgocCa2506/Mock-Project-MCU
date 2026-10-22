/*
 * boot_ota.h
 *
 *  OTA update: copy the OTA image (Sector 9-11) over the App (Sector 5-7).
 */

#ifndef INC_BOOT_OTA_H_
#define INC_BOOT_OTA_H_

#include <stdint.h>

/*
 * OTA method, select exactly one:
 *   1: write OTA binary over App code WITHOUT erasing the App sectors first
 *   2: erase App sectors first, then write OTA binary
 */
#define OTA_METHOD_OVERWRITE        1U
#define OTA_METHOD_ERASE_FIRST      2U

#define OTA_METHOD                  OTA_METHOD_ERASE_FIRST

/*
 * Benchmark results in CPU cycles (DWT). Each method fills its own variable.
 * Time [ms] = cycles / (SystemCoreClock / 1000), 16 MHz HSI -> cycles / 16000.
 */
extern volatile uint32_t g_ota_cycles_overwrite;     /* OTA_METHOD 1 */
extern volatile uint32_t g_ota_cycles_erase_write;   /* OTA_METHOD 2 */

/* Run OTA (blink orange LED), clear .ota_flag, copy binary and header. */
void boot_ota_run(void);

#endif /* INC_BOOT_OTA_H_ */

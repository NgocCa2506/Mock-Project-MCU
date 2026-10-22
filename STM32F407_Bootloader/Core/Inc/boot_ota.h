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
 *   2: erase App sectors first (all 3 sectors), then write OTA binary
 *   3: erase App sectors first (only sectors needed by app_size), then write
 */
#define OTA_METHOD_OVERWRITE            1U
#define OTA_METHOD_ERASE_FIRST          2U
#define OTA_METHOD_ERASE_SMART          3U

#define OTA_METHOD                      OTA_METHOD_ERASE_SMART

/*
 * Flash voltage range for erase and program operations, select exactly one:
 *
 *   FLASH_PROGRAM_VOLTAGE_RANGE_3  (2.7 V – 3.6 V)
 *     Erase  : FLASH_VOLTAGE_RANGE_3   (x32 parallelism)
 *     Program: FLASH_TYPEPROGRAM_WORD  (4 bytes per write)
 *
 *   FLASH_PROGRAM_VOLTAGE_RANGE_4  (2.7 V – 3.6 V + external Vpp 8–9 V)
 *     Erase  : FLASH_VOLTAGE_RANGE_4   (x64 parallelism)
 *     Program: FLASH_TYPEPROGRAM_DOUBLEWORD (8 bytes per write)
 *              NOTE: app_size must be a multiple of 8; the copy loop
 *              advances by 8 bytes at a time.
 */
#define FLASH_PROGRAM_VOLTAGE_RANGE_3   3U
#define FLASH_PROGRAM_VOLTAGE_RANGE_4   4U

#define FLASH_PROGRAM_VOLTAGE           FLASH_PROGRAM_VOLTAGE_RANGE_3

/*
 * Benchmark results in CPU cycles (DWT). Each method fills its own variable.
 * Time [ms] = cycles / (SystemCoreClock / 1000), 16 MHz HSI -> cycles / 16000.
 */
extern volatile uint32_t g_ota_cycles_overwrite;       /* OTA_METHOD 1 */
extern volatile uint32_t g_ota_cycles_erase_write;     /* OTA_METHOD 2 */
extern volatile uint32_t g_ota_cycles_erase_smart;     /* OTA_METHOD 3 */

/* Run OTA (blink orange LED), clear .ota_flag, copy binary and header. */
void boot_ota_run(void);

#endif /* INC_BOOT_OTA_H_ */

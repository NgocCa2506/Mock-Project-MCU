/*
 * app_header.c
 *
 *  Application Header instance, placed in the .app_header section
 *  (Flash sector 4, see flash_layout.h and the linker script).
 */

#include "app_header.h"
#include "app_config.h"

/*
 * The Magic Number is defined here. app_size and app_crc must be the size and
 * CRC-32 of application.bin (code only, produced by the post-build step).
 * After changing Application code: build, run Tools/app_bin_info.py on
 * Debug/application.bin, paste the result below, then build again.
 */
__attribute__((used, section(APP_HEADER_SECTION_NAME), aligned(4)))
const app_header_t g_app_header =
{
    .magic_number =
    {
        APP_MAGIC_WORD_0,
        APP_MAGIC_WORD_1,
        APP_MAGIC_WORD_2,
        APP_MAGIC_WORD_3,
        APP_MAGIC_WORD_4
    },
#if (APP_BUILD_OTA == 1U)
    /* OTA App: values of the OTA build */
    .app_size = 0x00001E14UL,
    .app_crc  = 0xE542A3DFUL,
#else
    /* Current App */
    .app_size = 0x00001E14UL,
    .app_crc  = 0x7FA60552UL,
#endif
    .version  = 1UL,
    .ota_flag = 0UL
};

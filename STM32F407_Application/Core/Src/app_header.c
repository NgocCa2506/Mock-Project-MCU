#include "app_header.h"

/*
 * app_size and app_crc are placeholders in the original ELF.
 * The post-build script calculates their real values from application.bin
 * and creates app_header.bin. objcopy then updates this section.
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
    .app_size = 0UL,
    .app_crc  = 0UL
};

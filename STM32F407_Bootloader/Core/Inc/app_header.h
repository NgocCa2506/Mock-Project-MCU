/*
 * app_header.h
 *
 *  Application Header format: Magic Number, App size and App CRC-32.
 *  NOTE: keep this file byte-identical in the Application and Bootloader
 *        projects. The Magic Number must be the same on both sides.
 */

#ifndef INC_APP_HEADER_H_
#define INC_APP_HEADER_H_

#include <stdint.h>

#define APP_HEADER_SECTION_NAME    ".app_header"
#define APP_MAGIC_WORD_COUNT       5U

#define APP_MAGIC_WORD_0           0x41505048UL
#define APP_MAGIC_WORD_1           0x53544D33UL
#define APP_MAGIC_WORD_2           0x32463430UL
#define APP_MAGIC_WORD_3           0x37424F4FUL
#define APP_MAGIC_WORD_4           0x54415050UL

typedef struct
{
    uint32_t magic_number[APP_MAGIC_WORD_COUNT];

    /*
     * Number of App Code bytes stored from APP_START_ADDRESS.
     */
    uint32_t app_size;

    /*
     * Expected CRC-32 of App Code.
     */
    uint32_t app_crc;

} app_header_t;

/*
 * Defined by the Application project only (app_header.c).
 * The Bootloader reads the header from APP_HEADER_START_ADDRESS instead.
 */
extern const app_header_t g_app_header;

#endif /* INC_APP_HEADER_H_ */

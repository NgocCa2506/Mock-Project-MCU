#ifndef INC_APP_HEADER_H_
#define INC_APP_HEADER_H_

#include <stdint.h>

#define APP_HEADER_SECTION_NAME    ".app_header"
#define APP_MAGIC_WORD_COUNT       5U

/* These values must be identical in both projects. */
#define APP_MAGIC_WORD_0           0x41505048UL
#define APP_MAGIC_WORD_1           0x53544D33UL
#define APP_MAGIC_WORD_2           0x32463430UL
#define APP_MAGIC_WORD_3           0x37424F4FUL
#define APP_MAGIC_WORD_4           0x54415050UL

typedef struct
{
    uint32_t magic_number[APP_MAGIC_WORD_COUNT];
    uint32_t app_size;
    uint32_t app_crc;
} app_header_t;

extern const app_header_t g_app_header;

#endif /* INC_APP_HEADER_H_ */

#ifndef INC_APP_HEADER_H_
#define INC_APP_HEADER_H_

#include <stdint.h>

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

typedef enum
{
    APP_HEADER_VALID = 0,
    APP_HEADER_MAGIC_INVALID,
    APP_HEADER_SIZE_INVALID,
    APP_HEADER_CRC_INVALID
} app_header_status_t;

const app_header_t *app_header_get(void);
app_header_status_t app_header_validate(void);
uint32_t app_header_calculate_crc32(const uint8_t *data, uint32_t length);

#endif /* INC_APP_HEADER_H_ */

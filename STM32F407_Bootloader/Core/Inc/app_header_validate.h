#ifndef INC_APP_HEADER_VALIDATE_H_
#define INC_APP_HEADER_VALIDATE_H_

#include <stdint.h>

#define APP_MAGIC_WORD_COUNT    5U

#define APP_MAGIC_WORD_0        0x41505048UL
#define APP_MAGIC_WORD_1        0x53544D33UL
#define APP_MAGIC_WORD_2        0x32463430UL
#define APP_MAGIC_WORD_3        0x37424F4FUL
#define APP_MAGIC_WORD_4        0x54415050UL

typedef struct
{
    uint32_t magic_number[APP_MAGIC_WORD_COUNT];
    uint32_t app_size;
    uint32_t app_crc;
    uint32_t version;
    uint32_t ota_flag;

} app_header_t;

typedef enum
{
    APP_HEADER_VALID = 0,

    APP_HEADER_MAGIC_INVALID,

    APP_HEADER_SIZE_INVALID,

    APP_HEADER_CRC_INVALID

} app_header_status_t;

/*
 * Return a pointer to the Application Header
 * stored at APP_HEADER_START_ADDRESS.
 */
const app_header_t *app_header_get(void);

/*
 * Validate:
 * - Magic Number[5]
 * - Application Size
 * - Application CRC
 */
app_header_status_t app_header_validate(void);

#endif /* INC_APP_HEADER_VALIDATE_H_ */

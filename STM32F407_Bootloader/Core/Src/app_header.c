#include "app_header.h"
#include "flash_layout.h"

#define CRC32_INITIAL_VALUE        0xFFFFFFFFUL
#define CRC32_FINAL_XOR_VALUE      0xFFFFFFFFUL
#define CRC32_REFLECTED_POLY       0xEDB88320UL

static const uint32_t expected_magic[APP_MAGIC_WORD_COUNT] =
{
    APP_MAGIC_WORD_0,
    APP_MAGIC_WORD_1,
    APP_MAGIC_WORD_2,
    APP_MAGIC_WORD_3,
    APP_MAGIC_WORD_4
};

const app_header_t *app_header_get(void)
{
    return (const app_header_t *)APP_HEADER_START_ADDRESS;
}

uint32_t app_header_calculate_crc32(const uint8_t *data, uint32_t length)
{
    uint32_t crc = CRC32_INITIAL_VALUE;
    uint32_t byte_index;
    uint32_t bit_index;

    for (byte_index = 0UL; byte_index < length; byte_index++)
    {
        crc ^= (uint32_t)data[byte_index];

        for (bit_index = 0UL; bit_index < 8UL; bit_index++)
        {
            if ((crc & 1UL) != 0UL)
            {
                crc = (crc >> 1U) ^ CRC32_REFLECTED_POLY;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ CRC32_FINAL_XOR_VALUE;
}

app_header_status_t app_header_validate(void)
{
    const app_header_t *header = app_header_get();
    uint32_t index;
    uint32_t app_end_address;
    uint32_t calculated_crc;

    /* Check the complete five-word Magic Number. */
    for (index = 0UL; index < APP_MAGIC_WORD_COUNT; index++)
    {
        if (header->magic_number[index] != expected_magic[index])
        {
            return APP_HEADER_MAGIC_INVALID;
        }
    }

    /* Validate the size stored in the Header before using it for CRC. */
    if ((header->app_size == 0UL) || (header->app_size > APP_SIZE))
    {
        return APP_HEADER_SIZE_INVALID;
    }

    app_end_address = APP_START_ADDRESS + header->app_size - 1UL;

    if (app_end_address > APP_END_ADDRESS)
    {
        return APP_HEADER_SIZE_INVALID;
    }

    /* Calculate CRC over App Code only, not over the Header. */
    calculated_crc = app_header_calculate_crc32(
        (const uint8_t *)APP_START_ADDRESS,
        header->app_size);

    if (calculated_crc != header->app_crc)
    {
        return APP_HEADER_CRC_INVALID;
    }

    return APP_HEADER_VALID;
}

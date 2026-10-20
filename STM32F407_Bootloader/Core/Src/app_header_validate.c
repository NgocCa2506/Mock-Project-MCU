/*
 * app_header_validate.c
 *
 * Read and validate the Application Header
 * stored at APP_HEADER_START_ADDRESS.
 */

#include "app_header_validate.h"
#include "flash_layout.h"
#include "crc32.h"

/*
 * Expected five-word Magic Number.
 */
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

app_header_status_t app_header_validate(void)
{
    const app_header_t *header;
    uint32_t index;
    uint32_t app_end_address;
    uint32_t calculated_crc;

    /*
     * Read Application Header from:
     * 0x08010000.
     */
    header = app_header_get();

    /*
     * 1. Check all five Magic Number elements.
     */
    for (index = 0UL;
         index < APP_MAGIC_WORD_COUNT;
         index++)
    {
        if (header->magic_number[index] !=
            expected_magic[index])
        {
            return APP_HEADER_MAGIC_INVALID;
        }
    }

    /*
     * 2. Application Size must be greater than zero
     * and must not exceed the Application region.
     */
    if ((header->app_size == 0UL) ||
        (header->app_size > APP_SIZE))
    {
        return APP_HEADER_SIZE_INVALID;
    }

    /*
     * Ensure app_size fits completely inside:
     *
     * 0x08020000 - 0x0807FFFF.
     */
    if (header->app_size >
        (APP_END_ADDRESS -
         APP_START_ADDRESS +
         1UL))
    {
        return APP_HEADER_SIZE_INVALID;
    }

    /*
     * Calculate the address of the last
     * Application byte.
     */
    app_end_address =
        APP_START_ADDRESS +
        header->app_size -
        1UL;

    if (app_end_address > APP_END_ADDRESS)
    {
        return APP_HEADER_SIZE_INVALID;
    }

    /*
     * 3. Calculate CRC over App Code only.
     *
     * Start address:
     *     APP_START_ADDRESS = 0x08020000
     *
     * Number of bytes:
     *     header->app_size
     *
     * crc32_calculate() selects the algorithm using:
     *     CRC_SELECTED_ALGORITHM.
     */
    calculated_crc =
        crc32_calculate(
            (const uint8_t *)APP_START_ADDRESS,
            header->app_size
        );

    /*
     * 4. Compare calculated CRC with the CRC
     * stored in Application Header.
     */
    if (calculated_crc != header->app_crc)
    {
        return APP_HEADER_CRC_INVALID;
    }

    return APP_HEADER_VALID;
}

/*
 * crc32.h
 *
 *  CRC-32 calculation (lookup-table implementation).
 *  Compatible with Python zlib.crc32(), which Tools/generate_app_header.py
 *  uses in the Application project.
 */

#ifndef INC_CRC32_H_
#define INC_CRC32_H_

#include <stdint.h>

/*
 * Calculate CRC-32 using the 256-entry lookup table.
 */
uint32_t crc32_byte(
    const uint8_t *data,
    uint32_t byte_length
);

#endif /* INC_CRC32_H_ */

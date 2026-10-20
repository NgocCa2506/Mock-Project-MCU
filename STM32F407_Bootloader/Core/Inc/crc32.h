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

/* Three available CRC-32/ISO-HDLC implementations. */
#define CRC_ALGORITHM_BITWISE       1U
#define CRC_ALGORITHM_LOOKUP_TABLE  2U
#define CRC_ALGORITHM_HARDWARE      3U

/* Select exactly one implementation for crc32_calculate(). */
#ifndef CRC_SELECTED_ALGORITHM
#define CRC_SELECTED_ALGORITHM     CRC_ALGORITHM_LOOKUP_TABLE
#endif

#if ((CRC_SELECTED_ALGORITHM != CRC_ALGORITHM_BITWISE) && \
     (CRC_SELECTED_ALGORITHM != CRC_ALGORITHM_LOOKUP_TABLE) && \
     (CRC_SELECTED_ALGORITHM != CRC_ALGORITHM_HARDWARE))
#error "Invalid CRC_SELECTED_ALGORITHM"
#endif

/* CRC-32/ISO-HDLC parameters, compatible with Python zlib.crc32(). */
#define CRC32_INITIAL_VALUE         0xFFFFFFFFUL
#define CRC32_FINAL_XOR_VALUE       0xFFFFFFFFUL
#define CRC32_REFLECTED_POLYNOMIAL  0xEDB88320UL

uint32_t crc32_bitwise(const uint8_t *data, uint32_t byte_length);
uint32_t crc32_lookup(const uint8_t *data, uint32_t byte_length);
uint32_t crc32_hardware(const uint8_t *data, uint32_t byte_length);

/* Compile-time selected implementation. */
uint32_t crc32_calculate(const uint8_t *data, uint32_t byte_length);

/* Optional runtime generation of the same 256-entry lookup table. */
void crc32_fill(uint32_t table[256]);

#endif /* INC_CRC32_H_ */

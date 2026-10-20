#ifndef INC_CRC_BENCHMARK_H_
#define INC_CRC_BENCHMARK_H_

#include <stdint.h>

typedef struct
{
    uint32_t crc_bitwise;
    uint32_t crc_lookup;
    uint32_t crc_hardware;

    uint32_t cycles_bitwise;
    uint32_t cycles_lookup;
    uint32_t cycles_hardware;

    uint8_t all_results_match;
} crc_benchmark_result_t;

void crc_benchmark_dwt_init(void);
void crc_benchmark_run(const uint8_t *data,
                       uint32_t byte_length,
                       crc_benchmark_result_t *result);

#endif /* INC_CRC_BENCHMARK_H_ */

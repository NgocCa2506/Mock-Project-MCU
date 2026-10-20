#include "crc_benchmark.h"
#include "crc32.h"
#include "stm32f4xx.h"

static uint32_t benchmark_one(uint32_t (*crc_function)(const uint8_t *, uint32_t),
                              const uint8_t *data,
                              uint32_t byte_length,
                              uint32_t *crc_result)
{
    uint32_t start_cycle;
    uint32_t end_cycle;

    __DSB();
    __ISB();
    start_cycle = DWT->CYCCNT;

    *crc_result = crc_function(data, byte_length);

    __DSB();
    __ISB();
    end_cycle = DWT->CYCCNT;

    return end_cycle - start_cycle;
}

void crc_benchmark_dwt_init(void)
{
    /* Enable trace and the Cortex-M4 cycle counter. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0UL;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    __DSB();
    __ISB();
}

void crc_benchmark_run(const uint8_t *data,
                       uint32_t byte_length,
                       crc_benchmark_result_t *result)
{
    if ((data == (const uint8_t *)0) ||
        (result == (crc_benchmark_result_t *)0))
    {
        return;
    }

    crc_benchmark_dwt_init();

    result->cycles_bitwise = benchmark_one(
        crc32_bitwise, data, byte_length, &result->crc_bitwise);

    result->cycles_lookup = benchmark_one(
        crc32_lookup, data, byte_length, &result->crc_lookup);

    result->cycles_hardware = benchmark_one(
        crc32_hardware, data, byte_length, &result->crc_hardware);

    result->all_results_match =
        ((result->crc_bitwise == result->crc_lookup) &&
         (result->crc_lookup == result->crc_hardware)) ? 1U : 0U;
}

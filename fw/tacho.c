#include <stdint.h>
#include <msp430.h>
#include "persist.h"

static void calculate_factor();

static volatile uint16_t tacho_counter = 0;

static uint8_t tacho_needs_inc()
{
    static uint8_t last;
    uint8_t cur = P1IN;
    uint8_t mask = 0;
    if (tacho_counter & 1) // expect rising edge on A1
        mask = BIT1;
    else // A3
        mask = BIT3;

    uint8_t r = (!(last & mask)) && (cur & mask);

    last = cur;

    return r;
}

void tacho_isr(void)
{
    if (tacho_needs_inc())
        tacho_counter++;
}

uint8_t tacho_get_wakeup()
{
    return tacho_needs_inc();
}

uint16_t tacho_counter_read(void)
{
    return tacho_counter;
}


static uint16_t factor;

uint8_t tacho_get_counts_per_revolution()
{
    return persist.tacho_counts_per_revolution;
}

void tacho_set_counts_per_revolution(uint8_t c)
{
    persist.tacho_counts_per_revolution = c;
    calculate_factor();
}

uint16_t tacho_get_circumference()
{
    return persist.tacho_circumference_mm;
}

void tacho_set_circumference(uint16_t c)
{
    persist.tacho_circumference_mm = c;
    calculate_factor();
}

static void calculate_factor()
{
    factor = (36L * persist.tacho_circumference_mm) / persist.tacho_counts_per_revolution;
}

static uint8_t tacho_kmh = 0;
static uint16_t total_m = 0;

static uint16_t mock_counts_per_second = 0;

void tacho_tick_handler()
{
    static uint16_t counts_last = 0;
    uint16_t counts = tacho_counter;
    uint16_t counts_delta = counts - counts_last;
    counts_delta += mock_counts_per_second;
    tacho_kmh = ((uint32_t)counts_delta * (uint32_t)factor + 5000) / 10000;

    static uint16_t remainder_mm = 0;
    static uint16_t counts_for_total = 0;
    counts_for_total += counts_delta;
    while (counts_for_total > persist.tacho_counts_per_revolution) {
        counts_for_total -= persist.tacho_counts_per_revolution;
        remainder_mm += persist.tacho_circumference_mm;
    }
    while (remainder_mm > 1000) {
        total_m++;
        remainder_mm -= 1000;
    }


    counts_last = counts;
}

uint8_t tacho_read_kmh()
{
    return tacho_kmh;
}

uint16_t tacho_read_total_m()
{
    return total_m;
}

void tacho_init()
{
    calculate_factor();
}

uint16_t tacho_get_mock_counts_per_second()
{
    return mock_counts_per_second;
}

void tacho_set_mock_counts_per_second(uint16_t cps)
{
    mock_counts_per_second = cps;
}

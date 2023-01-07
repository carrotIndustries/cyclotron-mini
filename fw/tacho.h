#pragma once
#include <stdint.h>

void tacho_isr(void);
uint16_t tacho_counter_read(void);

uint8_t tacho_get_counts_per_revolution();
void tacho_set_counts_per_revolution(uint8_t c);

uint16_t tacho_get_circumference();
void tacho_set_circumference(uint16_t c);

void tacho_tick_handler();
uint8_t tacho_read_kmh();
uint16_t tacho_read_total_m();
void tacho_init();
uint8_t tacho_get_wakeup();

uint16_t tacho_get_mock_counts_per_second();
void tacho_set_mock_counts_per_second(uint16_t cps);

uint16_t tacho_read_position_mm();

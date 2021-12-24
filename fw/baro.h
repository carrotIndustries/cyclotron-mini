#pragma once
#include <stdint.h>

void baro_tick_handler(void);
void baro_read(int32_t *temp_out, int32_t *press_out);
int32_t baro_read_press(void);
void baro_read_raw(uint32_t *d1_out, uint32_t *d2_out);
uint16_t baro_get_cal(uint8_t idx);

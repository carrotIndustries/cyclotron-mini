#pragma once

#include <stdint.h>

#define BUTTON_UP (1 << 4)
#define BUTTON_DOWN (1 << 5)
#define BUTTON_OK (1 << 6)
#define BUTTON_ALL (BUTTON_UP | BUTTON_DOWN | BUTTON_OK)

void button_init(void);
uint8_t button_isr(void);
uint8_t get_button_press(uint8_t button_mask);
uint8_t get_button_rpt(uint8_t button_mask);
uint8_t get_button_short(uint8_t button_mask);
uint8_t get_button_long(uint8_t button_mask);
uint8_t get_button_common(uint8_t button_mask);
uint8_t button_read_raw(void);

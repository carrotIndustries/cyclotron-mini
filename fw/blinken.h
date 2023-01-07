#pragma once
#include <stdint.h>

typedef enum {
    BLINKEN_PARAM_PATTERN,
    BLINKEN_PARAM_BRIGHNESS,
    BLINKEN_PARAM_SPEED_OVERRIDE,
    BLINKEN_PARAM_SPEED_MUL,
    N_BLINKEN_PARAM
} blinken_param_t;

extern const char* blinken_param_names[N_BLINKEN_PARAM];

void blinken_set_param(uint8_t param, uint8_t value);
uint8_t blinken_get_param(uint8_t param);
void blinken_set_param_pending(uint8_t param);
uint8_t blinken_get_tx_data();

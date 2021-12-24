#pragma once
#include <stdint.h>

void adc_init();
void adc_tick_handler();

typedef enum {
    ADC_CH_VIN,
    ADC_CH_VCAP,
    ADC_CH_VBUS,
    ADC_N_CHANNELS,
} adc_ch_t;
uint16_t adc_read_raw(uint8_t ch);
uint16_t adc_read_mv(uint8_t ch);
void adc_power_up();
void adc_power_down();

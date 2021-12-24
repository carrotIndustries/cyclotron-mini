#include "adc.h"
#include <msp430.h>
#include "util.h"

void adc_init()
{
    P1SEL0 |= (BIT6 | BIT6 | BIT7);
    ADCCTL2 = ADCRES_1 | ADCSR;
    ADCCTL1 = ADCSHP;
    adc_power_up();
}

void adc_power_up()
{
    ADCCTL0 = ADCSHT_2 | ADCON;
    PMMCTL0 = PMMPW | SVSHE;
    PMMCTL2 |= INTREFEN;
    PMMCTL0_H = 0;
}

static uint8_t state = 0;

void adc_power_down()
{
    ADCCTL0 = ADCSHT_2;
    ADCCTL0 &= ~ADCENC;
    PMMCTL0 = PMMPW | SVSHE;
    PMMCTL2 &= ~INTREFEN;
    PMMCTL0_H = 0;
    state = 0;
}

static uint16_t values[ADC_N_CHANNELS];

uint16_t adc_read_raw(uint8_t ch)
{
    return values[ch];
}

uint16_t adc_read_mv(uint8_t ch)
{
    uint32_t v = values[ch];

    return (v * 2190940UL) >> 18;
}

static const uint8_t adc_channels[] = {
        [ADC_CH_VIN] = 0xf7,
        [ADC_CH_VCAP] = 0x16,
        [ADC_CH_VBUS] = 0x35,
};

static void handle_st()
{
    uint8_t ch = state >> 1;
    uint8_t ch_info = adc_channels[ch];
    uint8_t ena = ch_info >> 4;
    uint8_t adc_ch = ch_info & 0xf;
    if (state & 1) {
        while (!(ADCIFG & ADCIFG0))
            ;
        values[ch] = ADCMEM0;
        if (ena != 0xf)
            P7OUT |= (1 << ena);
        ADCCTL0 &= ~ADCENC;

        if (ch == ADC_N_CHANNELS - 1)
            state = 0;
        else
            state++;
    }
    else {
        if (ena != 0xf)
            P7OUT &= ~(1 << ena);
        ADCMCTL0 = ADCSREF_1 | adc_ch;

        ADCCTL0 |= ADCSC | ADCENC;
        state |= 1;
    }
}

void adc_tick_handler()
{
    handle_st();
    if (!(state & 1))
        handle_st();
}

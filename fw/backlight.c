#include "backlight.h"
#include <msp430.h>
#include "event.h"
#include "persist.h"
#include "als.h"


static void backlight_set(uint8_t value)
{
    if (value) {
        TA1CCR1 = value;
        TA1CTL = TASSEL__SMCLK | MC__UP | TACLR;
        P4SEL0 |= BIT0;
    }
    else {
        TA1CTL = TASSEL__SMCLK | MC__STOP | TACLR;
        TA1CCR1 = 0;
        P4SEL0 &= ~BIT0;
        P4OUT |= BIT0;
    }
}


void backlight_init()
{
    TA1CCTL1 = OUTMOD_3;
    TA1CCR0 = 199;
    TA1CTL = TASSEL__SMCLK | MC__UP | TACLR;
    P4SEL0 |= BIT0;
    backlight_set(0);
}

void backlight_set_state(uint8_t st)
{
    if (st)
        backlight_set(persist.backlight_brightness);
    else
        backlight_set(0);
}

uint8_t backlight_get_state()
{
    return !!TA1CCR1;
}

void backlight_toggle(void)
{
    backlight_set_state(!backlight_get_state());
}

uint8_t backlight_get_brightness()
{
    return persist.backlight_brightness;
}

void backlight_set_brightness(uint8_t b)
{
    persist.backlight_brightness = b;
}

uint8_t backlight_get_threshold()
{
    return persist.backlight_threshold;
}

void backlight_set_threshold(uint8_t b)
{
    persist.backlight_threshold = b;
}

#define HYSTERESIS (5)

void backlight_handle_event(event_t ev)
{
    uint16_t als_ch0, als_ch1;
    als_read_raw(&als_ch0, &als_ch1);

    uint16_t thre_on = backlight_get_threshold() - HYSTERESIS;

    if (ev & EVENT_TICK) {
        uint16_t als_ch0, als_ch1;
        als_read_raw(&als_ch0, &als_ch1);

        static uint16_t last = 0;


        uint16_t thre_off = backlight_get_threshold() + HYSTERESIS;

        if (als_ch0 <= thre_on && last > thre_on)
            backlight_set_state(1);

        if (als_ch0 >= thre_off && last < thre_off)
            backlight_set_state(0);


        last = als_ch0;
    }
    else if (ev & EVENT_WAKEUP) {
        if (als_ch0 <= thre_on)
            backlight_set_state(1);
    }
}

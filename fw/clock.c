#include "clock.h"
#include <msp430.h>
#include "util.h"
#include "persist.h"

static hour_min_t the_clock __attribute__((noinit));

void clock_cal_set(int8_t ppm)
{
    persist.clock_cal_ppm = ppm;
}

int8_t clock_cal_get(void)
{
    return persist.clock_cal_ppm;
}

static void clock_inc()
{
    if (the_clock.m >= 59) {
        the_clock.m = 0;
        if (the_clock.h >= 23) {
            the_clock.h = 0;
        }
        else {
            the_clock.h++;
        }
    }
    else {
        the_clock.m++;
    }
}

static const uint16_t mod = 30719;

void __attribute__((interrupt((RTC_VECTOR)))) RTC_ISR(void)
{
    static uint8_t cal_div = 0;
    if (RTCIV == RTCIV_RTCIF) {
        if (cal_div >= 64) { // count to 65
            cal_div = 0;
        }
        else {
            cal_div++;
        }
        if (persist.clock_cal_ppm) {
            uint8_t cal_abs = ABS(persist.clock_cal_ppm);
            if (cal_div < cal_abs) {
                if (persist.clock_cal_ppm > 0) { // need to run faster, decrease mod
                    RTCMOD = mod - 2;
                }
                else {
                    RTCMOD = mod + 2;
                }
            }
        }
        else {
            RTCMOD = mod;
        }
        clock_inc();
    }
}

hour_min_t clock_get()
{
    __dint();
    hour_min_t r = the_clock;
    __eint();
    return r;
}

void clock_set(const hour_min_t c)
{
    __dint();
    the_clock = c;
    RTCCTL |= RTCSR;
    __eint();
}

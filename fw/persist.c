#include "persist.h"
#include <msp430.h>
#include <string.h>

persist_t persist __attribute__((section(".infoA")));

#define REVISION (2)

void persist_reset(void)
{
    memset(&persist, 0, sizeof(persist_t));
    persist.revision = REVISION;
    persist.lcd_contrast = 15;
    persist.standby_timeout = 2;
    persist.tacho_counts_per_revolution = 28;
    persist.tacho_circumference_mm = 2233;
    persist.backlight_brightness = 2;
    persist.alti_cal_pressure_Pa = 101325;
    persist.alti_cal_alti_m = 0;
    persist.trip_current = 0;
    persist.clock_cal_ppm = 0;
}

void persist_init(void)
{
    SYSCFG0 = PFWP;
    if (persist.revision != REVISION)
        persist_reset();
}

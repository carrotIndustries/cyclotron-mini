#pragma once
#include <stdint.h>
#include "alti.h"
#include "trip.h"

typedef struct {
    uint8_t revision;

    uint8_t lcd_contrast;
    uint8_t standby_timeout;

    uint8_t tacho_counts_per_revolution;
    uint16_t tacho_circumference_mm;

    uint8_t backlight_brightness;

    alti_place_t alti_places[ALTI_N_PLACES];

    int32_t alti_cal_pressure_Pa;
    int16_t alti_cal_alti_m;

    uint32_t total_m;

    trip_t trips[N_TRIPS];
    uint8_t trip_current;
    uint8_t backlight_threshold;
    int8_t clock_cal_ppm;
    uint32_t day_m;
} persist_t;

extern persist_t persist; // __attribute__((section(".infoA")));

void persist_init(void);
void persist_reset(void);

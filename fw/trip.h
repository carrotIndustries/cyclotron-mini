#pragma once
#include <stdint.h>
#include "clock.h"

void trip_handle_tick();
void trip_init();
void trip_new();

#define N_TRIPS (16)

typedef struct {
    uint32_t distance_m;
    uint32_t time_in_motion_s;
    uint32_t time_still_s;
    int16_t alti_start;
    int16_t alti_min;
    int16_t alti_max;
    uint16_t ascend;
    uint16_t descend;
    uint8_t vmax_kmh;
} trip_t;


trip_t *trip_get_current();
void trip_clear(trip_t *t);
uint8_t trip_get_avg_kmh(const trip_t *t);
uint16_t trip_get_avg_0p1kmh(const trip_t *t);
uint16_t trip_get_total_avg_0p1kmh(const trip_t *t);

hour_min_t trip_get_time_in_motion_h_m(const trip_t *t);
hour_min_t trip_get_time_still_h_m(const trip_t *t);
hour_min_t trip_get_time_total_h_m(const trip_t *t);

uint16_t trip_get_total_km();
void trip_set_total_km(uint16_t km);
void trip_reset_day_km();
uint32_t trip_get_day_m();

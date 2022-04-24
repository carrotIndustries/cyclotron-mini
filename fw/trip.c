#include "trip.h"
#include "tacho.h"
#include "util.h"
#include "baro.h"
#include "alti.h"
#include "persist.h"

trip_t *trip_current = persist.trips;

void trip_init()
{
    trip_current = &persist.trips[persist.trip_current];
}

void trip_new()
{
    if (trip_current->distance_m == 0)
        return;
    persist.trip_current++;
    if (persist.trip_current >= N_TRIPS)
        persist.trip_current = 0;
    trip_current = &persist.trips[persist.trip_current];
    trip_clear(trip_current);
}

static int16_t last_alti = 0;

void trip_clear(trip_t *t)
{
    t->distance_m = 0;
    t->vmax_kmh = 0;
    t->time_in_motion_s = 0;
    t->time_still_s = 0;
    t->alti_start = alti_get_baro();
    t->alti_max = t->alti_start;
    t->alti_min = t->alti_start;
    t->ascend = 0;
    t->descend = 0;
    last_alti = t->alti_start;
}

trip_t *trip_get_current()
{
    return trip_current;
}


#define ALTI_STEP (5)

void trip_handle_tick()
{
    static uint16_t total_m_last = 0;
    static uint16_t time_still_s = 0;
    uint16_t total_m = tacho_read_total_m();
    uint16_t delta_m = total_m - total_m_last;
    uint8_t kmh = tacho_read_kmh();

    if (!tacho_get_mock_counts_per_second()) // only add to total m if not mock
        persist.total_m += delta_m;

    persist.day_m += delta_m;
    trip_current->distance_m += delta_m;
    trip_current->vmax_kmh = MAX(trip_current->vmax_kmh, kmh);
    if (kmh) {
        if (trip_current->time_in_motion_s)
            trip_current->time_still_s += time_still_s;
        trip_current->time_in_motion_s++;
        time_still_s = 0;
    }
    else {
        time_still_s++;
    }
    total_m_last = total_m;

    int16_t alti = alti_get_baro();
    if (alti > trip_current->alti_max)
        trip_current->alti_max = alti;
    if (alti < trip_current->alti_min)
        trip_current->alti_min = alti;
    int16_t delta_alti = alti - last_alti;
    if (ABS(delta_alti) < 50) {
        if (ABS(delta_alti) > ALTI_STEP) {
            if (delta_alti > 0)
                trip_current->ascend += delta_alti;
            else
                trip_current->descend += -delta_alti;
            last_alti = alti;
        }
    }
    else {
        last_alti = alti;
    }
}

static uint16_t get_avg_0p1kmh(uint32_t t_s, uint32_t s_m)
{
    if (t_s == 0)
        return 0;
    return (s_m * 36UL) / t_s;
}

uint16_t trip_get_avg_0p1kmh(const trip_t *t)
{
    return get_avg_0p1kmh(t->time_in_motion_s, t->distance_m);
}

uint16_t trip_get_total_avg_0p1kmh(const trip_t *t)
{
    return get_avg_0p1kmh(t->time_in_motion_s + t->time_still_s, t->distance_m);
}

uint8_t trip_get_avg_kmh(const trip_t *t)
{
    return (trip_get_avg_0p1kmh(t) + 5) / 10;
}

uint32_t trip_get_time_total(const trip_t *t)
{
    return t->time_still_s + t->time_in_motion_s;
}

uint16_t trip_get_total_km()
{
    return persist.total_m / 1000;
}

void trip_set_total_km(uint16_t km)
{
    persist.total_m = (uint32_t)km * 1000UL;
}

void trip_reset_day_km()
{
    persist.day_m = 0;
}

uint32_t trip_get_day_m()
{
    return persist.day_m;
}

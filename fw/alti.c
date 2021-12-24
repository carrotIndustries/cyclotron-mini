#include "alti.h"
#include "persist.h"
#include "baro.h"
#include "persist.h"

alti_place_t *alti_get_place(uint8_t idx)
{
    if (idx >= ALTI_N_PLACES)
        return 0;
    return &persist.alti_places[idx];
}

static int32_t slope_num;
static int32_t slope_den;

static void calculate_slope(void)
{
    slope_num = persist.alti_cal_alti_m - 44330;
    slope_den = 5 * persist.alti_cal_pressure_Pa + persist.alti_cal_pressure_Pa / 4; // cal_pressure_Pa*5.25
}

void alti_cal(int32_t pressure_Pa, int16_t alti_m)
{
    persist.alti_cal_pressure_Pa = pressure_Pa;
    persist.alti_cal_alti_m = alti_m;
    calculate_slope();
}


void alti_init(void)
{
    calculate_slope();
}

int16_t alti_get(int32_t pressure_Pa)
{
    int32_t delta_pressure = pressure_Pa - persist.alti_cal_pressure_Pa;
    int32_t delta_alti = (delta_pressure * slope_num) / slope_den;
    return persist.alti_cal_alti_m + delta_alti;
}

int16_t alti_get_baro()
{
    return alti_get(baro_read_press());
}

#include "app.h"
#include "app/app.h"
#include "tacho.h"
#include "lcd.h"
#include "util.h"
#include "baro.h"
#include "trip.h"
#include "dash_menu.h"
#include "alti_cal.h"
#include "alti.h"
#include "power.h"
#include "review.h"
#include "clock.h"

typedef enum {
    READOUT_TEMP,
    READOUT_TOTAL_KM,
    READOUT_DAY_KM,
    READOUT_TRIP_KM,
    READOUT_TRIP_TIME_IN_MOTION,
    READOUT_CLOCK,
    READOUT_TRIP_TIME_TOTAL,
    READOUT_TRIP_TIME_STILL,
    READOUT_TRIP_VMAX,
    READOUT_TRIP_VAVG,
    READOUT_TRIP_VAVG_TOTAL,
    READOUT_ALTI,
    READOUT_ASC,
    READOUT_DESC,
    READOUT_ALTI_MAX,
    READOUT_ALTI_MIN,
    N_READOUTS,
} readout_t;


#define R(x) (1 << (READOUT_##x))
static const uint16_t readouts_available_in_motion = R(TEMP) | R(CLOCK) | R(TRIP_KM) | R(TRIP_VMAX) | R(TRIP_VAVG)
                                                     | R(TRIP_TIME_IN_MOTION) | R(ALTI) | R(ASC) | R(DESC);
static const uint16_t readouts_available_in_review = R(TRIP_KM) | R(TRIP_VMAX) | R(TRIP_VAVG) | R(TRIP_VAVG_TOTAL)
                                                     | R(TRIP_TIME_IN_MOTION) | R(TRIP_TIME_STILL) | R(TRIP_TIME_TOTAL)
                                                     | R(ASC) | R(DESC) | R(ALTI_MAX) | R(ALTI_MIN);
#undef R

static uint8_t readout_current = READOUT_TRIP_KM;

static void alti_puti(uint8_t pos, int16_t alti)
{
    if (alti >= 0)
        lcd_puti(pos, 4, alti);
    else
        lcd_puti_signed(pos, 4, alti);
}

static void vavg_puti(uint16_t avg)
{
    lcd_puti(4, 2, avg / 10);
    lcd_putc(6, '.');
    lcd_puti(7, 1, avg % 10);
}

trip_t *trip_review = 0;

static uint8_t readout_is_available(uint8_t kmh, uint8_t r)
{
    if (trip_review)
        return !!(readouts_available_in_review & (1 << r));
    if (kmh)
        return !!(readouts_available_in_motion & (1 << r));
    else
        return 1;
}

void put_trip_km(uint8_t pos, uint32_t distance_m)
{
    uint16_t trip_km = distance_m / 1000UL;

    if (trip_km < 1U) {
        lcd_putc(pos + 0, '.');
        lcd_puti_lz(pos + 1, 3, distance_m);
    }
    else if (trip_km < 10U) {
        lcd_puti(pos + 0, 1, trip_km);
        lcd_putc(pos + 1, '.');
        lcd_puti_lz(pos + 2, 2, (distance_m % 1000UL) / 10UL);
    }
    else if (trip_km < 100U) {
        lcd_puti(pos + 0, 2, trip_km);
        lcd_putc(pos + 2, '.');
        lcd_puti(pos + 3, 1, (distance_m % 1000UL) / 1000UL);
    }
    else {
        lcd_puti(pos + 0, 4, trip_km);
    }
}

static void put_trip_time(uint8_t pos, uint32_t sec)
{
    uint32_t min_total = sec / 60;
    uint8_t m = min_total % 60;
    uint8_t h = min_total / 60;
    uint8_t s = sec % 60;
    if (h) {
        lcd_puti(pos, 2, h);
        lcd_putc(pos + 2, ':');
        lcd_puti_lz(pos + 3, 2, m);
        lcd_putc(pos + 5, 'h');
    }
    else {
        lcd_puti(pos, 2, m);
        lcd_putc(pos + 2, ':');
        lcd_puti_lz(pos + 3, 2, s);
        lcd_putc(pos + 5, 'm');
    }
}


static void app_main(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    trip_t *trip;
    static uint8_t blink = 0;

    if (trip_review)
        trip = trip_review;
    else
        trip = trip_get_current();

    uint8_t kmh = tacho_read_kmh();
    if (event & EVENT_BUTTON_UP) {
        do {
            INC_MOD(readout_current, N_READOUTS);
        } while (!readout_is_available(kmh, readout_current));
    }
    else if (event & EVENT_BUTTON_DOWN) {
        do {
            DEC_MOD(readout_current, N_READOUTS);
        } while (!readout_is_available(kmh, readout_current));
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        if (trip_review) {
            trip_review = 0;
            app_set_view(&app_app_dash, VIEW_REVIEW_TRIP);
        }
        else {
            app_exit();
        }
    }
    else if (event & EVENT_BUTTON_OK) {
        if (trip_review) {
            trip_review = 0;
            app_set_view(&app_app_dash, VIEW_REVIEW_TRIP);
        }
        else {
            app_set_view(&app_app_dash, VIEW_MENU);
        }
    }
    else if (event & EVENT_BUTTON_DOWN_LONG) {
        power_down();
    }
    else if (event & EVENT_TICK) {
        blink = !blink;
    }

    if (!readout_is_available(kmh, readout_current))
        readout_current = READOUT_TRIP_KM;

    const uint8_t in_motion = kmh && !trip_review;

    switch (readout_current) {
    case READOUT_TRIP_KM: {
        if (in_motion) {
            put_trip_km(0, trip->distance_m);
            uint8_t avg_kmh = trip_get_avg_kmh(trip);
            lcd_putc(4, LCD_CUSTOM_CHAR_KM);
            if (kmh > avg_kmh) {
                lcd_putc(5, LCD_CUSTOM_CHAR_UP);
            }
            else if (kmh < avg_kmh) {
                lcd_putc(5, LCD_CUSTOM_CHAR_DOWN);
            }
            else if (kmh == avg_kmh) {
                lcd_putc(5, '-');
            }
        }
        else {
            uint16_t trip_km = trip->distance_m / 1000UL;
            lcd_puti(0, 3, trip_km);
            lcd_putc(3, '.');
            lcd_puti_lz(4, 3, trip->distance_m % 1000UL);
            lcd_putc(7, LCD_CUSTOM_CHAR_KM);
        }

    } break;

    case READOUT_TRIP_VMAX: {
        lcd_puts(0, "max");
        lcd_puti(3, 2, trip->vmax_kmh);
    } break;


    case READOUT_TOTAL_KM: {
        lcd_putc(0, 'T');
        lcd_puti(1, 6, trip_get_total_km());
        lcd_putc(7, LCD_CUSTOM_CHAR_KM);
    } break;


    case READOUT_DAY_KM: {
        lcd_putc(0, 'D');
        uint32_t m = trip_get_day_m();
        lcd_puti(1, 3, m / 1000UL);
        lcd_putc(4, '.');
        lcd_puti_lz(5, 2, (m % 1000UL) / 10U);
        lcd_putc(7, LCD_CUSTOM_CHAR_KM);
    } break;


    case READOUT_TEMP: {
        int32_t temp;
        baro_read(&temp, 0);
        lcd_puti_signed(0, 3, (temp + 50) / 100);
        lcd_putc(3, LCD_CUSTOM_CHAR_DEGREE);
    } break;

    case READOUT_ALTI: {
        int16_t alti = alti_get_baro();
        if (in_motion) {
            lcd_putc(0, 'A');
            alti_puti(1, alti);
        }
        else {
            lcd_puts(0, "Alt");
            alti_puti(4, alti);
        }
    } break;

    case READOUT_ASC: {
        if (in_motion) {
            lcd_putc(0, LCD_CUSTOM_CHAR_UP);
            lcd_puti(1, 4, trip->ascend);
        }
        else {
            lcd_puts(0, "Asc");
            lcd_puti(4, 4, trip->ascend);
        }
    } break;

    case READOUT_DESC: {
        if (in_motion) {
            lcd_putc(0, LCD_CUSTOM_CHAR_DOWN);
            lcd_puti(1, 4, trip->descend);
        }
        else {
            lcd_puts(0, "Dsc");
            lcd_puti(4, 4, trip->descend);
        }
    } break;

    case READOUT_ALTI_MAX: {
        lcd_putc(0, 'A');
        lcd_putc(1, LCD_CUSTOM_CHAR_UP);
        alti_puti(2, trip->alti_max);
    } break;

    case READOUT_ALTI_MIN: {
        lcd_putc(0, 'A');
        lcd_putc(1, LCD_CUSTOM_CHAR_DOWN);
        alti_puti(2, trip->alti_min);
    } break;

    case READOUT_TRIP_TIME_IN_MOTION: {
        if (!in_motion) {
            lcd_puts(0, "Tm");
            put_trip_time(2, trip->time_in_motion_s);
        }
        else {
            put_trip_time(0, trip->time_in_motion_s);
        }
    } break;

    case READOUT_CLOCK: {
        hour_min_t t = clock_get();
        lcd_puti(0, 2, t.h);
        if (blink)
            lcd_putc(2, ':');
        lcd_puti_lz(3, 2, t.m);
    } break;

    case READOUT_TRIP_TIME_STILL: {
        lcd_puts(0, "Ts");
        put_trip_time(2, trip->time_still_s);
    } break;

    case READOUT_TRIP_TIME_TOTAL: {
        lcd_puts(0, "Tt");
        put_trip_time(2, trip_get_time_total(trip));
    } break;

    case READOUT_TRIP_VAVG: {
        if (in_motion) {
            lcd_puts(0, "avg");
            lcd_puti(3, 2, trip_get_avg_kmh(trip));
        }
        else {
            lcd_puts(0, "Mavg");
            uint16_t avg = trip_get_avg_0p1kmh(trip);
            vavg_puti(avg);
        }
    } break;

    case READOUT_TRIP_VAVG_TOTAL: {
        lcd_puts(0, "Tavg");
        uint16_t avg = trip_get_total_avg_0p1kmh(trip);
        vavg_puti(avg);
    } break;

    case N_READOUTS:
        break;
    }
    if (in_motion)
        lcd_puti(6, 2, kmh);
}

static app_view_t views[] = {
        [VIEW_MAIN] = {.main = app_main},
        [VIEW_MENU] = {.main = dash_menu_main, .enter = dash_menu_enter},
        [VIEW_ALTI_CAL] = {.main = dash_alti_cal_main},
        [VIEW_REVIEW_TRIP] = {.main = dash_review_trip_main},
};

app_t app_app_dash = {
        .n_views = ARRAY_SIZE(views),
        .views = views,
        .view_current = 0,
};

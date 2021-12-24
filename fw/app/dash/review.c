#include "review.h"
#include "util.h"
#include "app.h"
#include "lcd.h"
#include "trip.h"
#include "persist.h"

static uint8_t trip_selected = 0;

typedef enum {
    MODE_SELECT,
    MODE_REVIEW,
} mode_t;

static uint8_t mode = MODE_SELECT;

static trip_t *get_trip_selected()
{
    return &persist.trips[((uint8_t)(persist.trip_current - trip_selected)) % N_TRIPS];
}

static void main_select(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    if (event & EVENT_BUTTON_UP) {
        INC_MOD(trip_selected, N_TRIPS);
    }
    else if (event & EVENT_BUTTON_DOWN) {
        DEC_MOD(trip_selected, N_TRIPS);
    }
    else if (event & EVENT_BUTTON_OK) {
        trip_review = get_trip_selected();
        app_set_view(&app_app_dash, VIEW_MAIN);
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        app_set_view(&app_app_dash, VIEW_MENU);
    }
    lcd_puti(0, 2, trip_selected);
    put_trip_km(3, get_trip_selected()->distance_m);

    lcd_putc(7, LCD_CUSTOM_CHAR_KM);
}

void dash_review_trip_main(uint8_t view, const app_t *app, event_t event)
{

    switch (mode) {
    case MODE_SELECT:
        main_select(view, app, event);
        break;

    default:;
    }
}

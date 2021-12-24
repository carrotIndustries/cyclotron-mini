#include "dash_menu.h"
#include "menu.h"
#include "util.h"
#include "app.h"
#include "trip.h"
#include "tacho.h"

static menu_state_t menu_state;

static void handler_trip_new(void *ud)
{

    trip_new();
    app_set_view(&app_app_dash, VIEW_MAIN);
}

static const menu_item_text_t menu_item_new_trip = {
        .text = "New Trip",
        .handler = handler_trip_new,
};

static void handler_trip_review(void *ud)
{
    app_set_view(&app_app_dash, VIEW_REVIEW_TRIP);
}

static const menu_item_text_t menu_item_review_trip = {
        .text = "RevuTrip",
        .handler = handler_trip_review,
};


static void handler_trip_reset(void *ud)
{
    trip_t *trip = trip_get_current();
    trip_clear(trip);
    app_set_view(&app_app_dash, VIEW_MAIN);
}

static const menu_item_text_t menu_item_reset_trip = {
        .text = "Rst Trip",
        .handler = handler_trip_reset,

};

static void handler_day_km_reset(void *ud)
{
    trip_reset_day_km();
    app_set_view(&app_app_dash, VIEW_MAIN);
}

static const menu_item_text_t menu_item_reset_day_km = {
        .text = "Rst Day\x1",
        .handler = handler_day_km_reset,

};

static void handler_alti_cal(void *ud)
{
    app_set_view(&app_app_dash, VIEW_ALTI_CAL);
}

static const menu_item_text_t menu_item_alti_cal = {
        .text = "Alti Cal",
        .handler = handler_alti_cal,
};

static int32_t mock_cps_get(void *ud)
{
    return tacho_get_mock_counts_per_second();
}

static void mock_cps_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = tacho_get_mock_counts_per_second();
    set_digit(&va, dig, dir, 0, 1000);
    tacho_set_mock_counts_per_second(va);
}

static const menu_item_adj_t menu_item_mock_counts_per_second = {
        .type = MENU_ITEM_T_ADJ,
        .header = "MCpS ",
        .text = "MockCpS",
        .digits = 3,
        .handler_get = mock_cps_get,
        .handler_set = mock_cps_set,
};

// the menu itself


static const menu_item_unknown_t *menu_items[] = {
        (void *)&menu_item_new_trip,     (void *)&menu_item_review_trip, (void *)&menu_item_reset_trip,
        (void *)&menu_item_reset_day_km, (void *)&menu_item_alti_cal,    (void *)&menu_item_mock_counts_per_second,
};

static void menu_exit(void)
{
    app_set_view(&app_app_dash, VIEW_MAIN);
}


static const menu_t menu = {
        .n_items = ARRAY_SIZE(menu_items),
        .items = (void *)menu_items,
        .handler_exit = menu_exit,
};


void dash_menu_main(uint8_t view, const app_t *app, event_t event)
{
    menu_run(&menu, &menu_state, event);
}

void dash_menu_enter(uint8_t view, const app_t *app)
{
    menu_state.item_current = 0;
}

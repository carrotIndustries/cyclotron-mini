#include "app/app.h"
#include "menu.h"
#include "util.h"
#include "lcd.h"
#include "backlight.h"
#include "tacho.h"
#include "power.h"
#include "trip.h"
#include "persist.h"
#include "spare.h"

static menu_state_t menu_state;
uint8_t debug_item_current;
uint16_t debug_value;

static void conf_exit(void)
{
    menu_state.item_current = 0;
    app_exit();
}

/* settings for backlight ****************************************************/
static int32_t c_backlight_threshold_get(void *ud)
{
    return backlight_get_threshold();
}

static void c_backlight_threshold_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = backlight_get_threshold();
    set_digit(&va, dig, dir, 1, 250);
    backlight_set_threshold(va);
}

static const menu_item_adj_t menu_item_backlight_timeout = {
        .type = MENU_ITEM_T_ADJ,
        .header = "Thre ",
        .text = "BlThre",
        .digits = 3,
        .handler_get = c_backlight_threshold_get,
        .handler_set = c_backlight_threshold_set,
};

static int32_t c_backlight_brightness_get(void *ud)
{
    return backlight_get_brightness();
}

static void c_backlight_brightness_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = backlight_get_brightness();
    set_digit(&va, dig, dir, 1, 199);
    backlight_set_brightness(va);
    backlight_set_state(1);
}

static uint8_t backlight_state_saved = 0;

static void c_backlight_brightness_enter(void *user_data)
{
    backlight_state_saved = backlight_get_state();
    backlight_set_state(1);
}

static void c_backlight_brightness_leave(void *user_data)
{
    backlight_set_state(backlight_state_saved);
}

static const menu_item_adj_t menu_item_backlight_brightness = {
        .type = MENU_ITEM_T_ADJ,
        .header = "Bri ",
        .text = "BlBright",
        .digits = 3,
        .handler_get = c_backlight_brightness_get,
        .handler_set = c_backlight_brightness_set,
        .handler_enter = c_backlight_brightness_enter,
        .handler_leave = c_backlight_brightness_leave,
};


/* settings for lcd **********************************************************/


static int32_t c_lcd_contrast_get(void *ud)
{
    return lcd_contrast_get();
}

static void c_lcd_contrast_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = lcd_contrast_get();
    set_digit(&va, dig, dir, 0, 31);
    lcd_contrast_set(va);
}

static const menu_item_adj_t menu_item_lcd_contrast = {
        .type = MENU_ITEM_T_ADJ,
        .header = "Contr ",
        .text = "Contrast",
        .digits = 2,
        .handler_get = c_lcd_contrast_get,
        .handler_set = c_lcd_contrast_set,
};

/* tacho counts/revolution **********************************************************/


static int32_t c_tacho_cpr_get(void *ud)
{
    return tacho_get_counts_per_revolution();
}

static void c_tacho_cpr_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = tacho_get_counts_per_revolution();
    set_digit(&va, dig, dir, 1, 255);
    tacho_set_counts_per_revolution(va);
}

static const menu_item_adj_t menu_item_tacho_cpr = {
        .type = MENU_ITEM_T_ADJ,
        .header = "C/R ",
        .text = "Cnts/Rev",
        .digits = 3,
        .handler_get = c_tacho_cpr_get,
        .handler_set = c_tacho_cpr_set,
};


/* tacho tire circumference **********************************************************/


static int32_t c_tacho_circ_get(void *ud)
{
    return tacho_get_circumference();
}

static void c_tacho_circ_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = tacho_get_circumference();
    set_digit(&va, dig, dir, 10, 9999);
    tacho_set_circumference(va);
}

static const menu_item_adj_t menu_item_tacho_circ = {
        .type = MENU_ITEM_T_ADJ,
        .header = "C/mm",
        .text = "Circ/mm",
        .digits = 4,
        .handler_get = c_tacho_circ_get,
        .handler_set = c_tacho_circ_set,
};

// standby timeout

static int32_t c_standby_timeout_get(void *ud)
{
    return power_get_standby_timeout();
}

static void c_standby_timeout_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = power_get_standby_timeout();
    set_digit(&va, dig, dir, 1, 15);
    power_set_standby_timeout(va);
}

static const menu_item_adj_t menu_item_standby_timeout = {
        .type = MENU_ITEM_T_ADJ,
        .header = "Stby ",
        .text = "StbyTime",
        .digits = 2,
        .handler_get = c_standby_timeout_get,
        .handler_set = c_standby_timeout_set,
};

// total km adjust

static int32_t c_total_km_get(void *ud)
{
    return trip_get_total_km();
}

static void c_total_km_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = trip_get_total_km();
    set_digit(&va, dig, dir, 1, 32000);
    trip_set_total_km(va);
}

static const menu_item_adj_t menu_item_total_km = {
        .type = MENU_ITEM_T_ADJ,
        .header = "Tkm",
        .text = "Total km",
        .digits = 5,
        .handler_get = c_total_km_get,
        .handler_set = c_total_km_set,
};

// clock

static hour_min_t clock_temp;

static int32_t c_clock_get(void *ud)
{
    return clock_temp.m + clock_temp.h * 100L;
}

static void c_clock_enter(void *ud)
{
    clock_temp = clock_get();
}

static void c_clock_leave(void *ud)
{
    clock_set(clock_temp);
}

static void c_clock_set(uint8_t dig, int8_t dir, void *user_data)
{
    switch (dig) {
    case 1:
    case 3:
        dir *= 10;
        break;
    }
    switch (dig) {
    case 3:
    case 2:
        clock_temp.h = CLAMP(clock_temp.h + dir, 0, 23);
        break;

    case 1:
    case 0:
        clock_temp.m = CLAMP(clock_temp.m + dir, 0, 59);
        break;

    default:
        return;
    }
}

static const menu_item_adj_t menu_item_clock = {
        .type = MENU_ITEM_T_ADJ,
        .header = "Time",
        .text = "Clock",
        .digits = 4,
        .handler_get = c_clock_get,
        .handler_set = c_clock_set,
        .handler_enter = c_clock_enter,
        .handler_leave = c_clock_leave,
};


// clock cal

static int32_t c_clock_cal_get(void *ud)
{
    return clock_cal_get();
}

static void c_clock_cal_set(uint8_t dig, int8_t dir, void *user_data)
{
    int16_t va = clock_cal_get();
    set_digit(&va, dig, dir, -60, 60);
    clock_cal_set(va);
}

static const menu_item_adj_t menu_item_clock_cal = {
        .type = MENU_ITEM_T_ADJ_SIGNED,
        .header = "CCal ",
        .text = "ClockCal",
        .digits = 3,
        .handler_get = c_clock_cal_get,
        .handler_set = c_clock_cal_set,
};
// spare mode

static uint8_t c_spare_mode_get(void *ud)
{
    return persist.spare_mode;
}

static void c_spare_mode_set(uint8_t choice, void *user_data)
{
    persist.spare_mode = choice;
}

static const menu_item_choice_t menu_item_spare_mode = {
        .type = MENU_ITEM_T_CHOICE,
        .text = "Sm ",
        .choice_pos = 3,
        .handler_get = c_spare_mode_get,
        .handler_set = c_spare_mode_set,
        .n_choices = 3,
        .choices =
                {
                        [SPARE_MODE_OFF] = "Off",
                        [SPARE_MODE_ILLUM] = "Illum",
                        [SPARE_MODE_BLINKEN] = "Blink",
                },
};

// the menu itself

static const menu_item_text_t *menu_items[] = {
        (void *)&menu_item_tacho_cpr,
        (void *)&menu_item_spare_mode,
        (void *)&menu_item_tacho_circ,
        (void *)&menu_item_backlight_timeout,
        (void *)&menu_item_backlight_brightness,
        (void *)&menu_item_lcd_contrast,
        (void *)&menu_item_standby_timeout,
        (void *)&menu_item_total_km,
        (void *)&menu_item_clock,
        (void *)&menu_item_clock_cal,
};

static const menu_t menu = {
        .n_items = ARRAY_SIZE(menu_items),
        .items = (void *)menu_items,
        .handler_exit = conf_exit,
};

static void app_main(uint8_t view, const app_t *app, event_t event)
{
    menu_run(&menu, &menu_state, event);
}

static app_view_t views[] = {
        {.main = app_main},
};

const app_t app_app_config = {
        .n_views = ARRAY_SIZE(views),
        .views = views,
};

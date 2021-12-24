#include "app/app.h"
#include "app/apps.h"
#include "menu.h"
#include "util.h"
#include "lcd.h"
#include "tacho.h"
#include "als.h"
#include "baro.h"
#include "adc.h"


typedef enum {
    DEBUG_TACH_COUNT,
    DEBUG_VIN,
    DEBUG_VCAP,
    DEBUG_VBUS,
    DEBUG_ALS_CH0,
    DEBUG_ALS_CH1,
    DEBUG_TEMP,
    DEBUG_PRESS,
    DEBUG_BARO_D1,
    DEBUG_BARO_D2,
    DEBUG_BARO_C1,
    DEBUG_BARO_C2,
    DEBUG_BARO_C3,
    DEBUG_BARO_C4,
    DEBUG_BARO_C5,
    DEBUG_BARO_C6,
    DEBUG_WDT_RESET,
} debug_item_t;

static uint8_t debug_item_current = DEBUG_VIN;
static uint16_t tacho_offset;


static const char *debug_items[] = {
        [DEBUG_TACH_COUNT] = "Tcnt", [DEBUG_VIN] = "Vin",    [DEBUG_VCAP] = "Vcap",          [DEBUG_VBUS] = "Vbus",
        [DEBUG_ALS_CH0] = "A0",      [DEBUG_ALS_CH1] = "A1", [DEBUG_TEMP] = "Tmp",           [DEBUG_PRESS] = "P",
        [DEBUG_BARO_D1] = "D1",      [DEBUG_BARO_D2] = "D2", [DEBUG_WDT_RESET] = "WDTReset",
};


static void app_main(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    if (event & EVENT_BUTTON_UP) {
        INC_MOD(debug_item_current, ARRAY_SIZE(debug_items));
    }
    else if (event & EVENT_BUTTON_DOWN) {
        DEC_MOD(debug_item_current, ARRAY_SIZE(debug_items));
    }
    else if (event & EVENT_BUTTON_OK) {
        switch (debug_item_current) {
        case DEBUG_TACH_COUNT:
            tacho_offset = tacho_counter_read();
            break;

        case DEBUG_WDT_RESET:
            lcd_puts(0, "Wait...");
            lcd_flush();
            while (1) {
            }

        default:;
        }
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        app_exit();
    }
    lcd_puts(0, debug_items[debug_item_current]);
    switch (debug_item_current) {

    case DEBUG_VIN:
        lcd_puti(4, 4, adc_read_mv(ADC_CH_VIN));
        break;

    case DEBUG_VCAP:
        lcd_puti(4, 4, adc_read_mv(ADC_CH_VCAP));
        break;

    case DEBUG_VBUS:
        lcd_puti(4, 4, adc_read_mv(ADC_CH_VBUS));
        break;

    case DEBUG_TACH_COUNT:
        lcd_puti(4, 4, tacho_counter_read() - tacho_offset);
        break;

    case DEBUG_ALS_CH0:
    case DEBUG_ALS_CH1: {
        uint16_t als_ch0, als_ch1;
        als_read_raw(&als_ch0, &als_ch1);
        if (debug_item_current == DEBUG_ALS_CH0)
            lcd_puti(3, 5, als_ch0);
        else
            lcd_puti(3, 5, als_ch1);
    } break;

    case DEBUG_TEMP:
    case DEBUG_PRESS: {
        int32_t temp, press;
        baro_read(&temp, &press);
        if (debug_item_current == DEBUG_TEMP)
            lcd_puti_signed(3, 5, temp);
        else
            lcd_puti(1, 7, press);
    } break;

    case DEBUG_BARO_D1:
    case DEBUG_BARO_D2: {
        uint32_t d1, d2;
        baro_read_raw(&d1, &d2);
        if (debug_item_current == DEBUG_BARO_D2)
            lcd_putix(2, 6, d2);
        else
            lcd_putix(2, 6, d1);

    } break;

    case DEBUG_BARO_C1:
    case DEBUG_BARO_C2:
    case DEBUG_BARO_C3:
    case DEBUG_BARO_C4:
    case DEBUG_BARO_C5:
    case DEBUG_BARO_C6: {
        uint8_t idx = debug_item_current - DEBUG_BARO_C1 + 1;
        lcd_putc(0, 'C');
        lcd_putc(1, '0' + idx);
        lcd_putc(2, '=');
        lcd_putix(3, 4, baro_get_cal(idx));
    } break;

    default:;
    }
}

static app_view_t view = {
        .main = app_main,
};

const app_t app_app_debug = {
        .n_views = 1,
        .views = &view,
};

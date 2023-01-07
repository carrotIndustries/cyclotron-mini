#include "app/app.h"
#include "app/apps.h"
#include "menu.h"
#include "util.h"
#include "lcd.h"
#include "blinken.h"


static uint8_t param_current = 0;
static uint8_t adj_digit = 0xff;

static void app_main(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    if(adj_digit == 0xff) {
        if (event & EVENT_BUTTON_UP) {
            INC_MOD(param_current, N_BLINKEN_PARAM);
        }
        else if (event & EVENT_BUTTON_DOWN) {
            DEC_MOD(param_current, N_BLINKEN_PARAM);
        }
    }
    else if(event & (EVENT_BUTTON_DOWN | EVENT_BUTTON_UP)) {
        int8_t dir = event & EVENT_BUTTON_UP ? 1 : -1;
        int16_t v = blinken_get_param(param_current);
        set_digit(&v, adj_digit, dir, 0, 255);
        blinken_set_param(param_current, v);
    }
    if (event & EVENT_BUTTON_OK) {
        switch(adj_digit) {
            case 0:
                adj_digit = 1;
            break;
            case 1:
                adj_digit = 2;
            break;
            case 2:
                adj_digit = 0xff;
            break;
            default:
                adj_digit = 0;
        }
            
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        app_exit();
    }
    lcd_puts(0, blinken_param_names[param_current]);
    lcd_puti_lz(5, 3, blinken_get_param(param_current));
    if(adj_digit != 0xff) {
        lcd_set_cursor(7-adj_digit);
    }
    else {
        lcd_set_cursor(LCD_CURSOR_OFF);
    }
}

static void app_enter(uint8_t view, const app_t *app)
{
    adj_digit = 0xff;
    for(uint8_t i = 0; i < N_BLINKEN_PARAM; i++)
    {
        blinken_set_param_pending(i);
    }
}

static app_view_t view = {
        .main = app_main,
        .enter = app_enter
};

const app_t app_app_blinken = {
        .n_views = 1,
        .views = &view,
};

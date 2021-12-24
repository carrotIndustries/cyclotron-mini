#include "alti_cal.h"
#include "menu.h"
#include "util.h"
#include "app.h"
#include "alti.h"
#include "lcd.h"
#include "baro.h"

static uint8_t item_current = 0;

typedef enum {
    MODE_SELECT,
    MODE_ACT,
    MODE_EDIT,
} mode_t;

static uint8_t mode = MODE_SELECT;

typedef enum { ACT_CAL, ACT_EDIT, N_ACTS } act_t;

static uint8_t act = ACT_CAL;

static void main_select(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    if (event & EVENT_BUTTON_UP) {
        INC_MOD(item_current, ALTI_N_PLACES);
    }
    else if (event & EVENT_BUTTON_DOWN) {
        DEC_MOD(item_current, ALTI_N_PLACES);
    }
    else if (event & EVENT_BUTTON_OK) {
        mode = MODE_ACT;
        act = ACT_CAL;
        app_set_redraw();
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        app_set_view(&app_app_dash, VIEW_MENU);
    }
    alti_place_t *pl = alti_get_place(item_current);
    lcd_putsn(0, ALTI_PLACE_LABEL_LENGTH, pl->label);
    lcd_puti(3, 4, pl->alti);
    lcd_putc(7, 'm');
}

static uint8_t cursor_pos = 0;

static void main_act(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    alti_place_t *pl = alti_get_place(item_current);
    if (event & EVENT_BUTTON_UP) {
        INC_MOD(act, N_ACTS);
    }
    else if (event & EVENT_BUTTON_DOWN) {
        DEC_MOD(act, N_ACTS);
    }
    else if (event & EVENT_BUTTON_OK) {
        switch (act) {
        case ACT_EDIT:
            mode = MODE_EDIT;
            cursor_pos = 0;
            app_set_redraw();
            return;

        case ACT_CAL:
            alti_cal(baro_read_press(), pl->alti);
            mode = MODE_SELECT;
            app_set_view(&app_app_dash, VIEW_MAIN);
            return;
        }
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        mode = MODE_SELECT;
        app_set_redraw();
    }
    switch (act) {
    case ACT_CAL:
        lcd_puts(0, "Cal");
        lcd_puti(3, 4, pl->alti);
        lcd_putc(7, 'm');
        break;

    case ACT_EDIT:
        lcd_puts(0, "Edit");
        lcd_putsn(5, ALTI_PLACE_LABEL_LENGTH, pl->label);
        break;
    }
}

static void inc_dec_char(char *c, int8_t inc)
{
    // space A...Z a...z 0...9
    if (*c == ' ') {
        if (inc > 0)
            *c = 'A';
        else
            *c = '9';
    }
    else if (*c == 'A' && inc < 0) {
        *c = ' ';
    }
    else if (*c == 'Z' && inc > 0) {
        *c = 'a';
    }
    else if (*c == 'a' && inc < 0) {
        *c = 'Z';
    }
    else if (*c == 'z' && inc > 0) {
        *c = '0';
    }
    else if (*c == '0' && inc < 0) {
        *c = 'z';
    }
    else if (*c == '9' && inc > 0) {
        *c = ' ';
    }
    else {
        *c += inc;
    }
}

static void inc_dec_place(int8_t inc)
{
    alti_place_t *pl = alti_get_place(item_current);
    if (cursor_pos < 3) { // editing label
        inc_dec_char(&pl->label[cursor_pos], inc);
    }
    else {
        uint8_t dig = 6 - cursor_pos;
        set_digit(&pl->alti, dig, inc, 0, 5000);
    }
}

static void main_edit(uint8_t view, const app_t *app, event_t event)
{
    lcd_clear();
    if (event & EVENT_BUTTON_UP) {
        inc_dec_place(1);
    }
    else if (event & EVENT_BUTTON_DOWN) {
        inc_dec_place(-1);
    }
    else if (event & EVENT_BUTTON_OK) {
        cursor_pos++;
        if (cursor_pos >= 7)
            cursor_pos = 0;
    }
    else if (event & EVENT_BUTTON_OK_LONG) {
        mode = MODE_SELECT;
        app_set_redraw();
    }
    alti_place_t *pl = alti_get_place(item_current);
    lcd_putsn(0, ALTI_PLACE_LABEL_LENGTH, pl->label);
    lcd_puti(3, 4, pl->alti);
    lcd_putc(7, 'm');
    lcd_set_cursor(cursor_pos);
}

void dash_alti_cal_main(uint8_t view, const app_t *app, event_t event)
{

    switch (mode) {
    case MODE_SELECT:
        main_select(view, app, event);
        break;

    case MODE_ACT:
        main_act(view, app, event);
        break;

    case MODE_EDIT:
        main_edit(view, app, event);
        break;

    default:;
    }
}

#include "menu.h"
#include "lcd.h"
#include "util.h"
#include "app/app.h"
#include <string.h>


void menu_run(const menu_t *menu, menu_state_t *state, event_t event)
{
    uint8_t *item_current = &(state->item_current);
    lcd_clear();
    if (state->adj_mode == 0) {
        if (event & EVENT_BUTTON_DOWN) {
            INC_MOD(*item_current, menu->n_items);
        }
        else if (event & EVENT_BUTTON_UP) {
            DEC_MOD(*item_current, menu->n_items);
        }
        else if (event & EVENT_BUTTON_OK_LONG) {
            if (menu->handler_exit) {
                menu->handler_exit();
            }
        }
        else if (event & EVENT_BUTTON_OK) {
            menu_item_unknown_t *it_u = menu->items[*item_current];
            switch (it_u->type) {
            case MENU_ITEM_T_TEXT: {
                menu_item_text_t *it = (void *)it_u;
                if (it->handler) {
                    it->handler(it->user_data);
                }
            } break;

            case MENU_ITEM_T_CHOICE: {
                menu_item_choice_t *it = (void *)it_u;
                uint8_t c = it->handler_get(it->user_data);
                INC_MOD(c, it->n_choices);
                if (it->handler_set) {
                    it->handler_set(c, it->user_data);
                }
            } break;

            case MENU_ITEM_T_ADJ:
            case MENU_ITEM_T_ADJ_SIGNED: {
                menu_item_adj_t *it = (void *)it_u;
                state->adj_mode = 1;
                state->adj_digit = it->digits - 1;
                if (it->handler_enter) {
                    it->handler_enter(it->user_data);
                }
                app_set_redraw();
            } break;
            }
        }
        menu_item_unknown_t *it_u = menu->items[*item_current];

        uint8_t header_len = 0;
        if (menu->header) {
            lcd_puts(0, menu->header);
            header_len = strlen(menu->header);
        }

        switch (it_u->type) {
        case MENU_ITEM_T_TEXT: {
            menu_item_text_t *it = (void *)it_u;
            lcd_puts(header_len, it->text);

        } break;
        case MENU_ITEM_T_CHOICE: {
            menu_item_choice_t *it = (void *)it_u;
            lcd_puts(header_len, it->text);
            uint8_t c = it->handler_get(it->user_data);
            if (it->choices[0][0] == 0) {
                lcd_puti(header_len + it->choice_pos, 2, c);
            }
            else {
                lcd_puts(header_len + it->choice_pos, it->choices[c]);
            }
        } break;
        case MENU_ITEM_T_ADJ:
        case MENU_ITEM_T_ADJ_SIGNED: {
            menu_item_adj_t *it = (void *)it_u;
            lcd_puts(header_len, it->text);
        } break;
        }
        // if (it_u->handler_draw) {
        //    it_u->handler_draw(state, it_u, it_u->user_data);
        //}
    }
    else {
        menu_item_adj_t *it = (void *)menu->items[*item_current];
        if (it->type != MENU_ITEM_T_ADJ && it->type != MENU_ITEM_T_ADJ_SIGNED) {
            state->adj_mode = 0;
            return;
        }

        if (event & EVENT_BUTTON_UP) {
            it->handler_set(it->digits - state->adj_digit - 1, 1, it->user_data);
        }
        else if (event & EVENT_BUTTON_DOWN) {
            it->handler_set(it->digits - state->adj_digit - 1, -1, it->user_data);
        }

        if (event & EVENT_BUTTON_OK_LONG) {
            state->adj_mode = 0;
            if (it->handler_leave) {
                it->handler_leave(it->user_data);
            }
        }

        uint8_t header_len = 0;
        if (it->header) {
            lcd_puts(0, it->header);
            header_len = strlen(it->header);
        }

        if (it->type == MENU_ITEM_T_ADJ)
            lcd_puti_lz(header_len, it->digits, it->handler_get(it->user_data));
        else
            lcd_puti_signed(header_len, it->digits, it->handler_get(it->user_data));
        if (event & EVENT_BUTTON_OK) {
            INC_MOD(state->adj_digit, it->digits);
            if (it->type == MENU_ITEM_T_ADJ_SIGNED) {
                if (state->adj_digit == 0)
                    state->adj_digit++;
            }
        }
        if (state->adj_digit < it->digits) {
            lcd_set_cursor(header_len + state->adj_digit);
        }
    }
}

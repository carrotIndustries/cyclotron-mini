#pragma once
#include <stdint.h>
#include "event.h"

typedef enum {
    MENU_ITEM_T_TEXT,
    MENU_ITEM_T_CHOICE,
    MENU_ITEM_T_ADJ,
    MENU_ITEM_T_ADJ_SIGNED,
} menu_item_type_t;

typedef struct {
    uint8_t item_current;
    uint8_t adj_mode;
    uint8_t adj_digit;
} menu_state_t;

struct menu_item_unknown_u;

typedef struct menu_item_unknown_u menu_item_unknown_t;

#define MENU_ITEM_COMMON                                                                                               \
    menu_item_type_t type;                                                                                             \
    void *user_data;                                                                                                   \
    //void (*handler_draw)(menu_state_t * state, menu_item_unknown_t * item, void *user_data);

struct menu_item_unknown_u {
    MENU_ITEM_COMMON
};

typedef struct {
    MENU_ITEM_COMMON
    const char *text;
    void (*handler)(void *user_data);
} menu_item_text_t;

typedef struct {
    MENU_ITEM_COMMON
    const char *text;
    void (*handler_set)(uint8_t choice, void *user_data);
    uint8_t (*handler_get)(void *user_data);
    uint8_t n_choices;
    uint8_t choice_pos;
    const char *choices[];
} menu_item_choice_t;

typedef struct {
    MENU_ITEM_COMMON
    const char *text;
    const char *header;
    void (*handler_set)(uint8_t dig, int8_t dir, void *user_data);
    int32_t (*handler_get)(void *user_data);
    void (*handler_enter)(void *user_data);
    void (*handler_leave)(void *user_data);
    uint8_t digits;
} menu_item_adj_t;

typedef struct {
    uint8_t n_items;
    menu_item_unknown_t **items;
    void (*handler_exit)(void);
    const char *header;
} menu_t;

void menu_run(const menu_t *menu, menu_state_t *state, event_t event);

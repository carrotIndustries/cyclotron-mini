#pragma once
#include <stdint.h>

typedef enum {
    EVENT_NONE = 0,
    EVENT_TICK = 1 << 0,
    EVENT_BUTTON_UP = 1 << 1,
    EVENT_BUTTON_DOWN = 1 << 2,
    EVENT_BUTTON_OK = 1 << 3,
    EVENT_BUTTON_UP_LONG = 1 << 4,
    EVENT_BUTTON_DOWN_LONG = 1 << 5,
    EVENT_BUTTON_OK_LONG = 1 << 6,
    EVENT_WAKEUP = 1 << 7
} event_t;

void handle_event(event_t event);

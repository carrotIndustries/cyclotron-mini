#pragma once
#include <stdint.h>
#include "event.h"

void backlight_init();
uint8_t backlight_get_state();
void backlight_set_state(uint8_t st);
uint8_t backlight_get_brightness();
void backlight_set_brightness(uint8_t b);
void backlight_toggle(void);

uint8_t backlight_get_threshold();
void backlight_set_threshold(uint8_t b);
void backlight_handle_event(event_t ev);

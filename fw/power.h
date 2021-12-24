#pragma once
#include <stdint.h>
#include "event.h"

void power_down(void);
uint8_t power_get_standby_timeout();
void power_set_standby_timeout(uint8_t c);
void power_handle_event(event_t ev);
void wdt_pet(void);

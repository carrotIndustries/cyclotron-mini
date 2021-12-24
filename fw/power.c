#include "power.h"
#include "tacho.h"
#include "event.h"
#include "persist.h"

uint8_t power_get_standby_timeout()
{
    return persist.standby_timeout;
}

void power_set_standby_timeout(uint8_t c)
{
    persist.standby_timeout = c;
}

void power_handle_event(event_t ev)
{
    static uint16_t timeout_counter = 0;
    if (tacho_read_kmh()) {
        timeout_counter = persist.standby_timeout * 60;
    }
    if (ev & (EVENT_BUTTON_DOWN | EVENT_BUTTON_UP | EVENT_BUTTON_OK)) {
        timeout_counter = persist.standby_timeout * 60;
    }
    if (ev & EVENT_TICK) {
        if (timeout_counter > 1) {
            timeout_counter--;
        }
        else if (timeout_counter == 1) {
            power_down();
            timeout_counter = 0;
        }
    }
}

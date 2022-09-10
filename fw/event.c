#include "app/app.h"
#include "event.h"
#include "als.h"
#include "baro.h"
#include "lcd.h"
#include "backlight.h"
#include "adc.h"
#include "tacho.h"
#include "trip.h"
#include "power.h"

void handle_event(event_t event)
{
    if (event & EVENT_TICK) {
        wdt_pet();
        als_tick_handler();
        baro_tick_handler();
        adc_tick_handler();
        tacho_tick_handler();
        trip_handle_tick();
    }
    if (event & EVENT_WAKEUP)
        trip_set_pause(1);

    if (event & EVENT_BUTTON_UP_LONG) {
        backlight_toggle();
    }
    backlight_handle_event(event);
    power_handle_event(event);
    while (1) {
        app_current_update();

        if (app_view_flags & APP_VIEW_FLAG_CHANGED) {
            if (app_current->views[app_current->view_current].enter) {
                app_current->views[app_current->view_current].enter(app_current->view_current, app_current);
            }
            app_view_flags = 0;
        }
        app_current->views[app_current->view_current].main(app_current->view_current, app_current, event);
        if (app_view_flags) {
            event = EVENT_NONE;
            app_view_flags &= ~APP_VIEW_FLAG_REDRAW;
        }
        else {
            return;
        }
    }
}

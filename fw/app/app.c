#include "apps.h"

#define APP_DEFAULT app_app_dash

const app_t *app_current = &APP_DEFAULT;
uint8_t app_view_flags = APP_VIEW_FLAG_CHANGED;
static const app_t *app_current_next = &APP_DEFAULT;

void app_launch(const app_t *app)
{
    app_current_next = app;
    if (app_current_next != app_current) {
        app_view_flags = APP_VIEW_FLAG_CHANGED;
    }
}

void app_exit(void)
{
    app_launch(&app_app_launcher);
}

void app_current_update(void)
{
    app_current = app_current_next;
}

void app_set_redraw(void)
{
    app_view_flags |= APP_VIEW_FLAG_REDRAW;
}

void app_set_view(app_t *app, uint8_t view)
{
    app->view_current = view;
    app_view_flags = APP_VIEW_FLAG_CHANGED;
}

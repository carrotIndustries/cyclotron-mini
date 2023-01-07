#include "app/app.h"
#include "app/apps.h"
#include "menu.h"
#include "util.h"
#include "power.h"

static menu_state_t menu_state;

static void launch(void *papp)
{
    app_t *app = papp;
    if (app)
        app_launch(app);
}

static void enter_standby(void *papp)
{
    power_down();
}

static void menu_time(void)
{
    app_launch(&app_app_dash);
}

static const menu_item_text_t menu_item0 = {
        .text = "Dash",
        .handler = launch,
        .user_data = (void *)&app_app_dash,
};

static const menu_item_text_t menu_item1 = {
        .text = "Config",
        .handler = launch,
        .user_data = (void *)&app_app_config,
};

static const menu_item_text_t menu_item2 = {
        .text = "Debug",
        .handler = launch,
        .user_data = (void *)&app_app_debug,
};

static const menu_item_text_t menu_item3 = {
        .text = "Standby",
        .handler = enter_standby,
};

static const menu_item_text_t menu_item4 = {
        .text = "Blinken",
        .handler = launch,
        .user_data = (void *)&app_app_blinken,
};

static const menu_item_unknown_t *menu_items[] = {
        (void *)&menu_item0,
        (void *)&menu_item1,
        (void *)&menu_item2,
        (void *)&menu_item3,
        (void *)&menu_item4,
};

static const menu_t menu = {
        .n_items = ARRAY_SIZE(menu_items),
        .items = (void *)menu_items,
        .handler_exit = menu_time,
        .header = ">",
};


static void app_main(uint8_t view, const app_t *app, event_t event)
{
    menu_run(&menu, &menu_state, event);
}

static app_view_t view = {
        .main = app_main,
};

const app_t app_app_launcher = {
        .n_views = 1,
        .views = &view,
};

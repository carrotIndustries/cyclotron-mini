#pragma once
#include <stdint.h>
#include "event.h"

struct app_u;
typedef struct app_u app_t;

typedef struct {
    void (*main)(uint8_t view, const app_t *app, event_t event);
    void (*enter)(uint8_t view, const app_t *app);
    // void (*leave)(uint8_t view, const app_t *app);
} app_view_t;

struct app_u {
    char *name;
    void (*enter)(const app_t *app);
    void (*leave)(const app_t *app);
    app_view_t *views;
    uint8_t n_views;
    uint8_t view_current;
};

extern const app_t *app_current;
extern uint8_t app_view_flags;

typedef enum {
    APP_VIEW_FLAG_CHANGED = (1 << 0),
    APP_VIEW_FLAG_REDRAW = (1 << 1),
} app_view_flag_t;

void app_launch(const app_t *app);
void app_exit(void);
void app_current_update(void);
void app_set_view(app_t *app, uint8_t view);
void app_set_redraw(void);

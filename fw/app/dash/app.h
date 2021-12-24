#pragma once
#include "app/app.h"
#include "trip.h"

extern app_t app_app_dash;

#define VIEW_MAIN (0)
#define VIEW_MENU (1)
#define VIEW_ALTI_CAL (2)
#define VIEW_REVIEW_TRIP (3)

void put_trip_km(uint8_t pos, uint32_t distance_m);

extern trip_t *trip_review;

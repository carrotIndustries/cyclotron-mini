#pragma once
#include <stdint.h>

typedef struct {
    uint8_t h;
    uint8_t m;
} hour_min_t;


void clock_init();
hour_min_t clock_get();
void clock_set(hour_min_t clk);

void clock_cal_set(int8_t ppm);
int8_t clock_cal_get(void);

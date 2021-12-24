#pragma once
#include <stdint.h>

#define ALTI_PLACE_LABEL_LENGTH (3)
#define ALTI_N_PLACES (4)

typedef struct {
    int16_t alti;
    char label[ALTI_PLACE_LABEL_LENGTH];
} alti_place_t;

void alti_cal(int32_t pressure_Pa, int16_t alti_m);
alti_place_t *alti_get_place(uint8_t idx);
int16_t alti_get(int32_t pressure_Pa);
int16_t alti_get_baro();
void alti_init(void);

#pragma once
#include <stdint.h>

void delay_10us(void);
void delay_50us(void);
void delay_ms(uint16_t ms);

void set_digit(int16_t *va, uint8_t dig, int8_t dir, int16_t mi, int16_t ma);

char hex2ascii(uint8_t v);


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define INC_MOD(x, mod)                                                                                                \
    do {                                                                                                               \
        if ((x) == (mod)-1) {                                                                                          \
            (x) = 0;                                                                                                   \
        }                                                                                                              \
        else {                                                                                                         \
            (x)++;                                                                                                     \
        }                                                                                                              \
    } while (0)

#define DEC_MOD(x, mod)                                                                                                \
    do {                                                                                                               \
        if ((x) == 0) {                                                                                                \
            (x) = (mod)-1;                                                                                             \
        }                                                                                                              \
        else {                                                                                                         \
            (x)--;                                                                                                     \
        }                                                                                                              \
    } while (0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define CLAMP_ABS(x, lim) (CLAMP((x), (-lim), (lim)))

#include "util.h"
#include <msp430.h>

void delay_10us(void)
{
    for (uint16_t i = 0; i < 16; i++) {
        __no_operation();
    }
}

void delay_50us(void)
{
    uint8_t i = 5;
    while (i--) {
        delay_10us();
    }
}

void delay_ms(uint16_t ms)
{
    while (ms--) {
        for (uint8_t i = 0; i < 100; i++) {
            delay_10us();
        }
    }
}

static int16_t pow_10(uint8_t dig)
{
    static const int16_t values[] = {1, 10, 100, 1000, 10000};
    return values[dig];
}

void set_digit(int16_t *va, uint8_t dig, int8_t dir, int16_t mi, int16_t ma)
{
    int16_t inc = dir * pow_10(dig);
    *va = CLAMP(*va + inc, mi, ma);
}

char hex2ascii(uint8_t v)
{
    if (v < 10) {
        return '0' + v;
    }
    else {
        return 'a' + v - 10;
    }
}

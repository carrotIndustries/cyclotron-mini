#include "baro.h"
#include "baro_i2c.h"
#include "util.h"
#include <msp430.h>

typedef enum {
    BARO_STATE_WAIT,
    BARO_STATE_INIT,
    BARO_STATE_D1,
    BARO_STATE_D2,
} baro_state_t;

static uint16_t caldata[8];
static uint32_t d1, d2;

#define CMD_PROM_READ (0xa0)
#define OSR_256 (0x00)
#define OSR_512 (0x02)
#define OSR_1024 (0x04)
#define OSR_2048 (0x06)
#define OSR_4096 (0x08)
#define OSR_8192 (0x0a)

#define CMD_CONVERT_D1(osr) ((0x40) | (osr))
#define CMD_CONVERT_D2(osr) ((0x50) | (osr))

#define CMD_ADC_READ (0x00)

static void read_cal()
{
    for (uint8_t i = 0; i < 7; i++) {
        baro_i2c_send(CMD_PROM_READ | (i << 1));
        uint8_t rxbuf[2];
        baro_i2c_recv(rxbuf, 2);
        caldata[i] = rxbuf[1] | ((uint16_t)rxbuf[0] << 8);
    }
}

uint16_t baro_get_cal(uint8_t idx)
{
    return caldata[idx];
}

static uint32_t read_adc()
{
    baro_i2c_send(CMD_ADC_READ);
    uint8_t rxbuf[3];
    baro_i2c_recv(rxbuf, 3);
    return (((uint32_t)rxbuf[0]) << 16) | ((uint32_t)rxbuf[1] << 8) | rxbuf[2];
}

void baro_read(int32_t *temp_out, int32_t *press_out)
{
    // uses 64 bit
    int32_t dT = (int32_t)d2 - (((int32_t)caldata[5]) * (1L << 8));

    if (temp_out)
        *temp_out = 2000L + (((int64_t)dT * (int64_t)caldata[6]) / (1LL << 23));

    if (press_out) {
        int64_t off = (((int64_t)caldata[2]) * (1LL << 17)) + ((((int64_t)caldata[4]) * dT) / (1LL << 6));
        int64_t sens = (((int64_t)caldata[1]) * (1LL << 16)) + ((((int64_t)caldata[3]) * dT) / (1 << 7));
        int32_t press = (((d1 * sens) / (1LL << 21)) - off) / (1LL << 15);
        *press_out = press;
    }

    /* int32_t dT = (d2 >> 8) - (int32_t)(caldata[5]);
     if (temp_out)
         *temp_out = 2000L + ((dT * (int32_t)caldata[6]) / (1LL << 15));

     if (press_out) {
         int32_t off = (((int32_t)caldata[2]) << 2) + ((((int32_t)caldata[2]) * dT) / (1 << 13));
         int32_t sens = ((int32_t)caldata[1]) + ((((int32_t)caldata[3]) * dT) / (1 << 15));

         *press_out = (((d1 >> 8) * sens) / (1UL << 12)) + (((d1 & 0xff) * sens) >> 20) - off;
     }
     */
}

int32_t baro_read_press(void)
{
    int32_t p;
    baro_read(0, &p);
    return p;
}

void baro_read_raw(uint32_t *d1_out, uint32_t *d2_out)
{
    *d1_out = d1;
    *d2_out = d2;
}

#define OSR OSR_8192

void baro_tick_handler(void)
{
    static uint8_t state = BARO_STATE_WAIT;
    switch (state) {
    case BARO_STATE_WAIT:
        state = BARO_STATE_INIT;
        break;

    case BARO_STATE_INIT:

        if (baro_i2c_send(0x1e)) { // reset error
            baro_i2c_reset();
        }
        else {
            read_cal();
            state = BARO_STATE_D1;
        }
        break;

    case BARO_STATE_D1:
        d2 = read_adc();
        // d2 = 0x77d1af;
        baro_i2c_send(CMD_CONVERT_D1(OSR));
        state = BARO_STATE_D2;
        break;

    case BARO_STATE_D2:
        d1 = read_adc();
        // d1 = 0x63f37e;
        baro_i2c_send(CMD_CONVERT_D2(OSR));
        state = BARO_STATE_D1;
        break;
    }
}

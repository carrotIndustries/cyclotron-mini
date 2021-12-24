#include "als.h"
#include "als_i2c.h"
#include <msp430.h>

#define I2C_ADDR (0x29)

void als_init(void)
{
    ALS_DIR |= (ALS_PWR | ALS_SCL | ALS_SDA);
    ALS_OUT &= ~(ALS_PWR | ALS_SCL | ALS_SDA);
    ALS_REN |= ALS_D;
}

void als_power_up(void)
{
    ALS_OUT |= (ALS_PWR);
    ALS_DIR &= ~ALS_D;
    ALS_OUT |= ALS_D; // enable pullups
}

void als_power_down(void)
{
    ALS_DIR |= (ALS_PWR | ALS_SCL | ALS_SDA);
    ALS_OUT &= ~(ALS_PWR | ALS_SCL | ALS_SDA);
}

uint8_t als_reg_read(uint8_t addr)
{
    als_i2c_write_byte(1, 0, I2C_ADDR << 1);
    als_i2c_write_byte(0, 0, addr);
    als_i2c_write_byte(1, 0, (I2C_ADDR << 1) | 1);
    uint8_t r = als_i2c_read_byte(1, 1);
    return r;
}

void als_reg_write(uint8_t addr, uint8_t value)
{
    als_i2c_write_byte(1, 0, I2C_ADDR << 1);
    als_i2c_write_byte(0, 0, addr);
    als_i2c_write_byte(0, 1, value);
}

#define ALS_REG_CONTR 0x80
#define ALS_REG_CONTR_ACTIVE (1 << 0)
#define ALS_REG_CONTR_GAIN_96 (0x07 << 2)

#define ALS_REG_CH1_LO 0x88
#define ALS_REG_CH1_HI 0x89
#define ALS_REG_CH0_LO 0x8A
#define ALS_REG_CH0_HI 0x8B


typedef enum {
    ALS_STATE_OFF,
    ALS_STATE_ON,
    ALS_STATE_ON_WAIT,
    ALS_STATE_ACTIVE,
} als_state_t;

static uint8_t als_state = ALS_STATE_OFF;

static uint16_t als_ch1, als_ch0;

void als_set_power_state(uint8_t s)
{
    if (s) {
        als_state = ALS_STATE_ON;
    }
    else {
        als_power_down();
        als_state = ALS_STATE_OFF;
    }
}

void als_tick_handler(void)
{
    switch (als_state) {
    case ALS_STATE_ON:
        als_power_up();
        als_state = ALS_STATE_ON_WAIT;
        break;

    case ALS_STATE_ON_WAIT:
        als_reg_write(ALS_REG_CONTR, ALS_REG_CONTR_ACTIVE | ALS_REG_CONTR_GAIN_96);
        als_state = ALS_STATE_ACTIVE;
        break;

    case ALS_STATE_ACTIVE:
        als_ch1 = als_reg_read(ALS_REG_CH1_LO);
        als_ch1 |= ((uint16_t)als_reg_read(ALS_REG_CH1_HI)) << 8;
        als_ch0 = als_reg_read(ALS_REG_CH0_LO);
        als_ch0 |= ((uint16_t)als_reg_read(ALS_REG_CH0_HI)) << 8;
        break;

    case ALS_STATE_OFF:
        break;
    }
}

void als_read_raw(uint16_t *ch0, uint16_t *ch1)
{
    *ch0 = als_ch0;
    *ch1 = als_ch1;
}

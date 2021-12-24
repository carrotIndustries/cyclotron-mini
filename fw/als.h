#pragma once
#include <stdint.h>

#define ALS_DIR P3DIR
#define ALS_OUT P3OUT
#define ALS_IN P3IN
#define ALS_REN P3REN
#define ALS_PWR (BIT6)
#define ALS_SDA (BIT3)
#define ALS_SCL (BIT5)
#define ALS_D   (ALS_SDA | ALS_SCL)

void als_init(void);
void als_power_up(void);
void als_power_down(void);
uint8_t als_reg_read(uint8_t addr);
void als_reg_write(uint8_t addr, uint8_t value);
void als_tick_handler(void);
void als_read(uint16_t *ch0, uint16_t *ch1);
void als_set_power_state(uint8_t s);
void als_read_raw(uint16_t *ch0, uint16_t *ch1);

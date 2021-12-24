#pragma once
#include <stdint.h>

void baro_i2c_init();
uint8_t baro_i2c_send(uint8_t cmd);
void baro_i2c_reset(void);
void baro_i2c_recv(uint8_t *buf, uint8_t size);

#pragma once
#include <stdint.h>

void lcd_spi_init(void);
void lcd_spi_deinit(void);
void lcd_spi_send(uint8_t b);

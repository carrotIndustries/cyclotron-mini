#pragma once
#include <stdint.h>

void lcd_init(void);
void lcd_flush(void);

void lcd_putc(uint8_t pos, char c);
void lcd_send_cmd(uint8_t d);
void lcd_puts(uint8_t pos, const char *s);
void lcd_putsn(uint8_t pos, uint8_t len, const char *s);
void lcd_puti(uint8_t pos, uint8_t len, uint32_t value);
void lcd_puti_lz(uint8_t pos, uint8_t len, uint32_t value);
void lcd_puti_signed(uint8_t dig, uint8_t len, int32_t value);

void lcd_clear(void);
#define LCD_CURSOR_OFF 0xff
void lcd_set_cursor(uint8_t pos);


uint8_t lcd_contrast_get();
void lcd_contrast_set(uint8_t c);
void lcd_putix(uint8_t dig, uint8_t len, uint32_t value);
void lcd_power_up();
void lcd_power_down(void);

#define LCD_CUSTOM_CHAR_DEGREE (0)
#define LCD_CUSTOM_CHAR_KM (1)
#define LCD_CUSTOM_CHAR_UP (2)
#define LCD_CUSTOM_CHAR_DOWN (3)
#define LCD_CUSTOM_CHAR_AVG (4)

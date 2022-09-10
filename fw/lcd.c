#include "lcd.h"
#include <msp430.h>
#include "lcd_spi.h"
#include "util.h"
#include <string.h>
#include "persist.h"

#define LCD_RS_OUT P7OUT
#define LCD_RS_BIT BIT4

#define LCD_LENGTH (8)

static char lcd_buf[LCD_LENGTH];
static uint8_t cursor_pos = LCD_CURSOR_OFF;

void lcd_send_cmd(uint8_t d)
{
    LCD_RS_OUT &= ~LCD_RS_BIT;
    lcd_spi_send(d);
}

void lcd_send_data(uint8_t d)
{
    LCD_RS_OUT |= LCD_RS_BIT;
    lcd_spi_send(d);
    delay_10us();
}

uint8_t lcd_contrast_get()
{
    return persist.lcd_contrast;
}

static void apply_contrast()
{
    lcd_send_cmd(0x31);
    delay_50us();
    lcd_send_cmd(0x54 | ((persist.lcd_contrast >> 4) & 3));
    delay_50us();
    lcd_send_cmd(0x70 | (persist.lcd_contrast & 0xf));
    delay_50us();
    lcd_send_cmd(0x30);
    delay_50us();
}

void lcd_contrast_set(uint8_t c)
{

    persist.lcd_contrast = c;
    apply_contrast();
}

void lcd_power_up(void)
{
    P3OUT |= BIT0;
    delay_ms(100);
    lcd_spi_init();
    delay_ms(1);
    lcd_send_cmd(1);
    delay_ms(1);
    lcd_init();
}

void lcd_power_down(void)
{
    lcd_spi_deinit();
    LCD_RS_OUT &= ~LCD_RS_BIT;
    P3OUT &= ~BIT0;
}

static const uint8_t custom_char_data[] = {
        // degree C
        0b10000,
        0b00000,
        0b01111,
        0b10000,
        0b10000,
        0b10000,
        0b01111,
        0b00000,

        // km
        0b10000,
        0b10010,
        0b11100,
        0b10010,
        0b00000,
        0b11111,
        0b10101,
        0b00000,

        // up arrow
        0b00100,
        0b01110,
        0b11111,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,

        // down arrow
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b01110,
        0b00100,
        0b00000,

        // avg
        0b00000,
        0b00001,
        0b01110,
        0b01110,
        0b01110,
        0b10000,
        0b00000,
        0b00000,
};

static void lcd_init_custom_chars()
{
    lcd_send_cmd(0x40);
    delay_50us();

    for (uint16_t i = 0; i < ARRAY_SIZE(custom_char_data); i++) {
        lcd_send_data(custom_char_data[i]);
    }
}

void lcd_init(void)
{
    lcd_send_cmd(0x31);
    delay_50us();
    lcd_send_cmd(0x14);
    delay_50us();
    lcd_send_cmd(0x55);
    delay_50us();
    lcd_send_cmd(0x6d);
    delay_ms(5);
    lcd_send_cmd(0x70);
    delay_50us();
    lcd_send_cmd(0x30);
    delay_50us();
    lcd_send_cmd(0x0c);
    delay_50us();
    lcd_send_cmd(0x01);
    delay_ms(5);
    lcd_send_cmd(0x06);
    delay_50us();
    if (persist.lcd_contrast > 31)
        persist.lcd_contrast = 16;
    apply_contrast();

    lcd_init_custom_chars();
}

void lcd_flush(void)
{
    lcd_send_cmd(0x02); // home
    delay_50us();
    for (uint8_t i = 0; i < LCD_LENGTH; i++)
        lcd_send_data(lcd_buf[i]);
    if (cursor_pos == LCD_CURSOR_OFF) {
        lcd_send_cmd(0x0c);
        delay_50us();
    }
    else {
        lcd_send_cmd(0x80 | cursor_pos);
        delay_50us();
        lcd_send_cmd(0x0e);
        delay_50us();
    }
}

void lcd_putc(uint8_t pos, char c)
{
    if (pos >= LCD_LENGTH)
        return;
    lcd_buf[pos] = c;
}

void lcd_puts(uint8_t pos, const char *s)
{
    while (*s) {
        lcd_putc(pos++, *s++);
    }
}

void lcd_putsn(uint8_t pos, uint8_t len, const char *s)
{
    while (len--) {
        lcd_putc(pos++, *s++);
    }
}

void lcd_set_cursor(uint8_t pos)
{
    cursor_pos = pos;
}

void lcd_clear(void)
{
    memset(lcd_buf, ' ', LCD_LENGTH);
    cursor_pos = LCD_CURSOR_OFF;
}

static void lcd_puti_lz_opt(uint8_t pos, uint8_t len, uint32_t value, uint8_t lz)
{
    pos += len - 1;
    if (!lz)
        lcd_putc(pos, '0');
    while (len--) {
        if (value || lz)
            lcd_putc(pos, '0' + (value % 10));
        value /= 10;
        pos--;
    }
}

void lcd_puti(uint8_t pos, uint8_t len, uint32_t value)
{
    lcd_puti_lz_opt(pos, len, value, 0);
}

void lcd_puti_lz(uint8_t pos, uint8_t len, uint32_t value)
{
    lcd_puti_lz_opt(pos, len, value, 1);
}

void lcd_puti_signed(uint8_t dig, uint8_t len, int32_t value)
{
    if (value < 0) {
        value *= -1;
        lcd_putc(dig, '-');
    }
    else {
        lcd_putc(dig, '+');
    }
    lcd_puti(dig + 1, len - 1, value);
}


static char hex2ascii(uint8_t v)
{
    if (v < 10) {
        return '0' + v;
    }
    else {
        return 'a' + v - 10;
    }
}

void lcd_putix(uint8_t dig, uint8_t len, uint32_t value)
{
    dig += len - 1;
    while (len--) {
        lcd_putc(dig, hex2ascii(value & 0xf));
        value >>= 4;
        dig--;
    }
}

#include <msp430.h>
#include "lcd_spi.h"

#define CS_OUT P7OUT
#define CS_BIT BIT5

void lcd_spi_init(void)
{
    P1DIR |= BIT0 | BIT2;
    P1OUT &= ~(BIT0 | BIT2);
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCMSB | UCMST | UCSYNC | UCSSEL__SMCLK | UCCKPL;
    UCA0BRW = 8;
    UCA0CTLW0 &= ~UCSWRST;
    P1SEL0 |= BIT0 | BIT2;
}

void lcd_spi_deinit(void)
{
    P1SEL0 &= ~(BIT0 | BIT2);
    CS_OUT &= ~CS_BIT;
}

void lcd_spi_send(uint8_t b)
{
    CS_OUT &= ~CS_BIT;
    UCA0TXBUF = b;
    while (UCA0STATW & UCBUSY)
        ;
    CS_OUT |= CS_BIT;
}

#include <msp430.h>
#include "baro_i2c.h"
#include "util.h"

void baro_i2c_reset(void)
{
    UCB0CTL1 |= UCSWRST;
    delay_10us();
    UCB0CTL1 &= ~UCSWRST;
}

uint8_t baro_i2c_send(uint8_t cmd)
{
    UCB0CTL1 |= UCSWRST;
    UCB0CTLW0 |= UCTR;
    UCB0TBCNT = 0x1;
    UCB0CTL1 &= ~UCSWRST;

    UCB0CTLW0 |= UCTXSTT;
    UCB0IFG &= ~UCSTPIFG;
    uint8_t timeout = 200;

    while (UCB0CTLW0 & UCTXSTT) {
        if (timeout) {
            timeout--;
            delay_50us();
        }
        else {
            return 1;
        }
    }

    UCB0TXBUF = cmd;
    timeout = 200;
    while (!(UCB0IFG & UCSTPIFG)) {
        if (timeout) {
            timeout--;
            delay_50us();
        }
        else {
            return 1;
        }
    }

    return 0;
}

void baro_i2c_recv(uint8_t *buf, uint8_t size)
{
    UCB0CTL1 |= UCSWRST;
    UCB0CTLW0 &= ~UCTR;
    UCB0TBCNT = size;
    UCB0CTL1 &= ~UCSWRST;

    UCB0IFG &= ~(UCSTPIFG | UCRXIFG0);
    UCB0CTLW0 |= UCTXSTT;

    while (UCB0CTLW0 & UCTXSTT)
        ;

    while (size--) {
        while (!(UCB0IFG & UCRXIFG0))
            ;
        UCB0IFG &= ~UCRXIFG0;
        *buf++ = UCB0RXBUF;
    }

    while (!(UCB0IFG & UCSTPIFG))
        ;
}

void baro_i2c_init()
{
    UCB0CTL1 |= UCSWRST;
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCTR;
    UCB0BRW = 0x0080;
    UCB0CTLW1 = UCASTP_2;
    UCB0TBCNT = 0x1;
    UCB0I2CSA = 0x76;
    P5SEL0 |= BIT2 | BIT3;
    P5OUT |= (BIT2 | BIT3);
    P5REN |= BIT2 | BIT3;
    P5DIR &= ~(BIT2 | BIT3);

    UCB0CTL1 &= ~UCSWRST;
    /*while (1) {
        baro_i2c_send(0xa0);
        baro_i2c_recv(rxbuf, 2);
        for (uint16_t i = 0; i < 1000; i++) {
            __no_operation();
        }
    }*/
}

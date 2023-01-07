#include <msp430.h>
#include <stdint.h>
#include "als.h"
#include "baro_i2c.h"
#include "lcd_spi.h"
#include "lcd.h"
#include "button.h"
#include "util.h"
#include "event.h"
#include "tacho.h"
#include "backlight.h"
#include "adc.h"
#include "alti.h"
#include "power.h"
#include "persist.h"
#include "blinken.h"
#include "spare.h"

volatile uint8_t reg_value;

void tick_init()
{
    TA0CCR0 = 31;
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR | TAIE;
}

volatile uint8_t tick_event = 0;
volatile uint8_t global_power_on = 1;

// static const char *txbuf = "Hello World";

#define SPARE_OUT P1OUT
#define SPARE_BIT BIT4

static void uart_tx_bit()
{
    static uint8_t state = 0;
    static uint8_t dout = 0;
    if (state == 0) { // start bit
        SPARE_OUT &= ~SPARE_BIT;
        dout = blinken_get_tx_data();
        state++;
    }
    else if (state >= 1 && state <= 8) // data bits
    {
        if (dout & 1)
            SPARE_OUT |= SPARE_BIT;
        else
            SPARE_OUT &= ~SPARE_BIT;
        dout >>= 1;
        state++;
    }
    else // stop bit
    {
        SPARE_OUT |= SPARE_BIT;
        state++;
        if (state == 19)
            state = 0;
    }
}

void __attribute__((interrupt((TIMER0_A1_VECTOR)))) TA_ISR(void)
{
    static uint8_t debounce_divider = 0;
    static uint16_t tick_divider = 0;
    static uint8_t power_on_freeze = 8;
    if (TA0IV == TA0IV_TAIFG) {
        if (!global_power_on) {
            wdt_pet();
            if (power_on_freeze) {
                power_on_freeze--;
                return;
            }

            static uint8_t sec_div = 3;
            if (sec_div == 0) {
                trip_inc_time_still();
                sec_div = 3;
            }
            else {
                sec_div--;
            }

            if (button_read_raw() & BUTTON_ALL)
                LPM3_EXIT;

            if (tacho_get_wakeup())
                LPM3_EXIT;

            return;
        }

        switch (persist.spare_mode) {
        case SPARE_MODE_BLINKEN:
            uart_tx_bit();
            break;

        case SPARE_MODE_ILLUM:
            if (backlight_get_state())
                SPARE_OUT |= SPARE_BIT;
            else
                SPARE_OUT &= ~SPARE_BIT;
            break;

        default: // off
            SPARE_OUT &= ~SPARE_BIT;
        }

        power_on_freeze = 8;
        tacho_isr();

        if (debounce_divider) {
            debounce_divider--;
        }
        else {
            debounce_divider = 9;
            if (button_isr())
                LPM3_EXIT;
        }

        if (tick_divider) {
            tick_divider--;
        }
        else {
            tick_divider = 1023;
            tick_event = 1;
            LPM3_EXIT;
        }
    }
}

void power_down()
{
    global_power_on = 0;
}

void wdt_pet()
{
    WDTCTL = WDTPW | WDTSSEL__ACLK | WDTCNTCL | WDTIS__512K;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;
    P4SEL0 |= BIT1 | BIT2;
    while (SFRIFG1 & OFIFG) {
        CSCTL7 &= ~XT1OFFG;
        CSCTL7 &= ~DCOFFG;
        SFRIFG1 &= ~OFIFG;
    }
    CSCTL4 = SELA__XT1CLK;

    _bis_SR_register(SCG0);
    CSCTL0 = 0;
    CSCTL1 = DCORSEL_3;
    CSCTL2 = 244 | FLLD__1;
    _no_operation();
    _no_operation();
    _no_operation();
    _bic_SR_register(SCG0);
    while (CSCTL7 & FLLUNLOCK_3)
        ;

    RTCCTL = RTCPS__64;
    RTCMOD = 30719;

    RTCCTL |= RTCSS__XT1CLK | RTCSR | RTCIE;


    SYSCFG2 |= ADCPCTL5 | ADCPCTL6 | ADCPCTL7;
    P2DIR = 0xff;
    P3DIR = 0xff;
    P4DIR = 0xff;
    P5DIR = ~(BIT2 | BIT3);
    P6DIR = 0xff;
    P7DIR = 0xff;
    P8DIR = 0xff;
    P4OUT |= BIT0;
    P1DIR = BIT0 | BIT2 | BIT4;
    P1OUT ^= BIT4;

    tick_init();
    persist_init();
    backlight_init();

    als_init();
    als_power_down();
    baro_i2c_init();
    adc_init();
    button_init();
    tacho_init();
    alti_init();
    trip_init();


    // als_power_up();

    lcd_power_up();

    /*lcd_send_data('A');
    lcd_send_data('B');
    lcd_send_data('C');
    lcd_send_data('x');
    lcd_send_data(' ');
    lcd_send_data('2');
    lcd_send_data('4');
    lcd_send_data(0);
    lcd_send_cmd(0x10);
    delay_50us();
    lcd_send_cmd(0x40);
    delay_50us();
    lcd_send_data(0x07 << 1);
    lcd_send_data(0x04 << 1);
    lcd_send_data(0x07 << 1);
    lcd_send_data(0x01 << 1);
    lcd_send_data(0x07 << 1);
    lcd_send_data(0x00);
    lcd_send_data(0x10);*/

    // lcd_send_cmd(0x08);

    wdt_pet();

    __eint();
    als_set_power_state(1);


    while (1) {
        event_t ev = 0;
        if (!global_power_on) {
            als_set_power_state(1);
            lcd_power_up();
            adc_power_up();
            TA0CCR0 = 31;
            global_power_on = 1;
            ev |= EVENT_WAKEUP;
        }


#define BUTTON_TO_EV_SHORT(b)                                                                                          \
    do {                                                                                                               \
        if (get_button_short(BUTTON_##b)) {                                                                            \
            ev |= EVENT_BUTTON_##b;                                                                                    \
        }                                                                                                              \
    } while (0)
#define BUTTON_TO_EV_LONG(b)                                                                                           \
    do {                                                                                                               \
        if (get_button_long(BUTTON_##b)) {                                                                             \
            ev |= EVENT_BUTTON_##b##_LONG;                                                                             \
        }                                                                                                              \
    } while (0)
        BUTTON_TO_EV_SHORT(OK);
        BUTTON_TO_EV_SHORT(UP);
        BUTTON_TO_EV_SHORT(DOWN);
        BUTTON_TO_EV_LONG(OK);
        BUTTON_TO_EV_LONG(UP);
        BUTTON_TO_EV_LONG(DOWN);
#undef BUTTON_TO_EV_SHORT
#undef BUTTON_TO_EV_LONG

        if (tick_event) {
            tick_event = 0;
            ev |= EVENT_TICK;
        }


        handle_event(ev);
        lcd_flush();

        if (!global_power_on) {
            als_set_power_state(0);
            lcd_power_down();
            backlight_set_state(0);
            adc_power_down();
            SPARE_OUT &= ~SPARE_BIT;
            TA0CCR0 = 8191;
        }
        LPM3;
    }


    return 0;
}

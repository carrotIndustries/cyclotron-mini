#include "button.h"
#include <msp430.h>

#define REPEAT_MASK (BUTTON_ALL)
#define REPEAT_START 50 /* 500 ms */
#define REPEAT_NEXT 20  /* 200 ms */

volatile uint8_t button_state;
volatile uint8_t button_press;
volatile uint8_t button_rpt;

#define BUTTON_PIN P2IN
#define BUTTON_DIR P2DIR
#define BUTTON_RSEL P2REN
#define BUTTON_OUT P2OUT

void button_init()
{
    BUTTON_DIR &= ~BUTTON_ALL;
    BUTTON_RSEL |= BUTTON_ALL;
    BUTTON_OUT |= BUTTON_ALL;
}

uint8_t button_read_raw(void)
{
    return (~BUTTON_PIN) & BUTTON_ALL;
}

uint8_t button_isr()
{
    static uint8_t ct0 = 0xFF, ct1 = 0xFF, rpt;
    uint8_t i;

    i = button_state ^ ~BUTTON_PIN;
    ct0 = ~(ct0 & i);
    ct1 = ct0 ^ (ct1 & i);
    i &= ct0 & ct1;
    button_state ^= i;
    button_press |= button_state & i;

    if ((button_state & REPEAT_MASK) == 0)
        rpt = REPEAT_START;
    if (--rpt == 0) {
        rpt = REPEAT_NEXT;
        button_rpt |= button_state & REPEAT_MASK;
    }

    return (button_press | button_state | button_rpt) & BUTTON_ALL;
}


uint8_t get_button_press(uint8_t button_mask)
{
    __dint();
    __no_operation();
    button_mask &= button_press;
    button_press ^= button_mask;
    __nop();
    __eint();
    return button_mask;
}

uint8_t get_button_rpt(uint8_t button_mask)
{
    __dint();
    __no_operation();
    button_mask &= button_rpt;
    button_rpt ^= button_mask;
    __nop();
    __eint();
    return button_mask;
}

uint8_t get_button_short(uint8_t button_mask)
{
    __dint();
    __no_operation();
    return get_button_press(~button_state & button_mask);
}

uint8_t get_button_long(uint8_t button_mask)
{
    return get_button_press(get_button_rpt(button_mask));
}

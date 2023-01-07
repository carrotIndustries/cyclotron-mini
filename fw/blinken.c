#include "blinken.h"
#include "tacho.h"
#include "util.h"
#include <msp430.h>

const char* blinken_param_names[N_BLINKEN_PARAM] = {
    [BLINKEN_PARAM_PATTERN] = "Ptrn",
    [BLINKEN_PARAM_BRIGHNESS] = "Brn",
    [BLINKEN_PARAM_SPEED_OVERRIDE] = "SpOv",
    [BLINKEN_PARAM_SPEED_MUL] = "SMul",
};

static uint8_t blinken_params[N_BLINKEN_PARAM] = {
  [BLINKEN_PARAM_PATTERN] = 0,  
  [BLINKEN_PARAM_BRIGHNESS] = 0xf0,
  [BLINKEN_PARAM_SPEED_MUL] = 0xff,
};
static uint8_t blinken_params_pending = 0;

static uint8_t get_wheel_pos()
{
    static uint16_t mm_last;
    static uint16_t mm_acc;
    static uint8_t wheel_pos;
    uint16_t mm = tacho_read_position_mm();
    uint16_t delta = mm-mm_last;
    mm_last = mm;
    mm_acc += delta;
    while(mm_acc >= 5) {
        mm_acc -= 5;
        wheel_pos++;
    }
    return wheel_pos&0x7f;
}

uint8_t blinken_get_param(uint8_t param)
{
    return blinken_params[param];
}

void blinken_set_param(uint8_t param, uint8_t value)
{
    __dint();
    __no_operation();
    blinken_params[param] = value;
    blinken_params_pending |= (1<<param);
    __nop();
    __eint();
}

void blinken_set_param_pending(uint8_t param)
{
    __dint();
    __no_operation();
    blinken_params_pending |= (1<<param);
    __nop();
    __eint();
}

typedef enum {
    TX_STATE_START,
    TX_STATE_D0,
    TX_STATE_D1,
} tx_state_t;

static uint8_t find_lowest_bit_set_and_clear(uint8_t *v)
{
    uint8_t t = 1;
    for(uint8_t i = 0; i<8; i++) {
        if(*v & t) {
            *v &= ~t;
            return i;
        }
        t <<= 1;
    }
    return 0xff;
}


uint8_t blinken_get_tx_data()
{
    static uint8_t last_sent_wheel_pos = 0;
    static uint8_t last_wheel_pos;
    static uint8_t state;
    static uint8_t param_value;
    
    uint8_t p = get_wheel_pos();
    if(!(blinken_params_pending || (state != TX_STATE_START)) || (last_wheel_pos!=p && !last_sent_wheel_pos)) {
        last_wheel_pos = p;
        last_sent_wheel_pos = 1;
        return p+0x80;
    }
    else  {
        last_sent_wheel_pos = 0;
        switch(state) {
            case TX_STATE_D0:
                state = TX_STATE_D1;
                return hex2ascii((param_value>>4)&0xf);
            break;
            
            case TX_STATE_D1:
                state = TX_STATE_START;
                return hex2ascii(param_value&0xf);
            break;
            
            default:
            {
                uint8_t param = find_lowest_bit_set_and_clear(&blinken_params_pending);
                if(param != 0xff) {
                    last_sent_wheel_pos = 0;
                    param_value = blinken_params[param];
                    state = TX_STATE_D0;
                    return 'A' + param;
                }
            }
        }
        return 0x00; 
    } 
}

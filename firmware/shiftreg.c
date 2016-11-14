#include "ch.h"
#include "hal.h"

#include "shiftreg.h"

void shiftreg_init(){
    // Clear the shift register
    shiftreg_send(0,32);
}

//TODO: do these need extra delay to make pulses longer?
static void shiftreg_clock_bit(uint32_t bit){
    if(bit != 0){
        palSetLine(LINE_SHIFT_DATA);
    }else{
        palClearLine(LINE_SHIFT_DATA);
    }
    
    palSetLine(LINE_SHIFT_CLK);
    palClearLine(LINE_SHIFT_CLK);
}

static void shiftreg_latch(void){
    palSetLine(LINE_SHIFT_LATCH);
    palClearLine(LINE_SHIFT_LATCH);
}

void shiftreg_send(uint32_t databits, uint32_t nbits){
    uint32_t i=0;
    if(nbits > 32) nbits = 32;
    for(i=0; i<nbits; i++){
        shiftreg_clock_bit(databits & 0x1);
        databits >>= 1;
    }
    shiftreg_latch();
}

#include "ch.h"
#include "hal.h"

#include "shiftreg.h"
#include "display.h"
#include "font.h"

// Each entry is a column, with LSB = top row
uint8_t screen[5];
uint8_t leds;

void display_init(){
    shiftreg_init();
    leds = 0;
    int i=0;
    for(i=0; i<5; i++){
        screen[i] = 0;
    }
}

void display_update(){
    // Show each column of the screen, with a 1ms delay between each
    int i;
    for(i=0; i<5; i++){
        uint8_t col = ~(1 << (7-i)); // set one column to LOW
        uint8_t row = (screen[i] & 0x1f) << 1;
        uint32_t shift_pattern = leds | (col<<8) | (row<<16);
        
        shiftreg_send(shift_pattern, 24);
        
        chThdSleepMilliseconds(1);
    }
}

void screen_show_char(char c){
    uint32_t val = 0;
    if(c>=' ' && c <= '~')
        val = FONT_TABLE[c-' '];
    
    int col;
    for(col=0; col<5; col++){
        screen[col] = val >> (5*col);
    }
}

void led_turn_on(int which){
    leds |= (1<<which);
}

void led_turn_off(int which){
    leds &= ~(1<<which);
}

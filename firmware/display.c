#include "ch.h"
#include "hal.h"

#include "shiftreg.h"
#include "display.h"
#include "font.h"

static const char message[] = "HO HO HO MERRY CHRISTMAS  ";

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

void display_show(){
    // Show each column of the screen, with a 1ms delay between each
    int i;
    for(i=0; i<5; i++){
        uint8_t col = ~(1 << (4-i)); // set one column to LOW
        uint8_t row = (screen[i] & 0x1f) << 2;
        uint32_t shift_pattern = leds | (col<<8) | (row<<16);
        
        shiftreg_send(shift_pattern, 24);
        
        chThdSleepMilliseconds(1);
    }
}

void display_update(){
    static uint32_t i = 0;

    uint32_t charnum = i / 6;
    uint8_t column = i % 6;
    uint32_t fontchar;
    if (message[charnum] != 0){
        fontchar  = FONT_TABLE[message[charnum]-' '];
    }else{
        fontchar = 0;
    }

    i += 1;
    if(i >= 6*sizeof(message)){
        i = 0;
    }

    // Scroll the current screen along by 1 column
    int j;
    for(j=4; j>0; j--){
        screen[j] = screen[j-1];
    }
    if(column < 5){ // Leave a blank column at the end
        screen[0] = (fontchar >> ((4-column)*5));
    }else{
        screen[0] = 0;
    }
}

void led_turn_on(int which){
    leds |= (1<<which);
}

void led_turn_off(int which){
    leds &= ~(1<<which);
}

#include "ch.h"
#include "hal.h"

#include "shiftreg.h"
#include "display.h"
#include "font.h"
#include "animations.h"

// Each entry is a column, with LSB = bottom row
uint8_t screen[5];
uint8_t leds;

#define MAX_BRIGHTNESS  8
// Ordering of LEDS:     4 3 2 5
//                       8 7 6 1
uint8_t brightness[8] = {MAX_BRIGHTNESS, 1, 1, 1,
                         1, MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS};
uint8_t pwm_tick = 0;

void display_init(){
    shiftreg_init();

    leds = 255;

    display_clear();
}

void display_show(){
    int i;

    // Show each column of the screen, with a 1ms delay between each
    for(i=0; i<5; i++){
        // Update PWM of LEDs
        uint8_t pwm_mask = 0;

        pwm_tick += 1;
        if (pwm_tick >= MAX_BRIGHTNESS){
            pwm_tick = 0;
        }

        int j;
        for(j=0; j<8; j++){
            if (pwm_tick < brightness[j]){
                pwm_mask |= 1 << j;
            }
        }

        uint8_t col = ~(1 << (4-i)); // set one column to LOW
        uint8_t row = (screen[i] & 0x1f) << 2;
        uint32_t shift_pattern = (leds & pwm_mask) | (col<<8) | (row<<16);

        shiftreg_send(shift_pattern, 24);

        chThdSleepMilliseconds(1);
    }
}

static char message[255];
void display_scroll_text(const char *str){
    const char *p = str;
    char *d = message;
    while(*p){
        *d++ = *p++;
    }
    *d = 0; // Terminate with a null-byte
}

bool display_scroll(){
    static uint32_t i = 0;

    bool retval = false;

    uint32_t charnum = i / 6;
    uint8_t column = i % 6;
    uint32_t fontchar;
    i += 1;
    if (message[charnum] != 0){
        fontchar  = FONT_TABLE[message[charnum]-' '];
    }else{
        fontchar = 0;
        if(column == 5){
            i = 0;
            retval = true; // Return true if we're done scrolling
        }
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

    return retval;
}

static uint8_t animidx = 0;
static uint32_t frame = 0;
void display_set_anim(uint8_t idx){
    if(idx < NUM_ANIMS){
        animidx = idx;
        frame = 0;
    }
}

bool display_anim(){
    bool retval = false;
    frame += 1;
    if (frame >= ANIM_LENS[animidx]){
        frame = 0;
        retval = true; // Return true if the animation is over
    }

    int i;
    for(i=0; i<5; i++){
        screen[i] = ANIMS[animidx][frame] >> (i*5);
    }
    return retval;
}

void display_clear(){
    int i;
    for(i=0; i<5; i++){
        screen[i] = 0;
    }
}

void led_turn_on(int which){
    leds |= (1<<which);
}

void led_turn_off(int which){
    leds &= ~(1<<which);
}

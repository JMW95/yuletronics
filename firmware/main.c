#include "ch.h"
#include "hal.h"

#include "rand.h"
#include "display.h"
#include "animations.h"

static const char *messages[] = {
    "MERRY CHRISTMAS",
    "HO HO HO",
    "JINGLE BELLS",
    "SANTA CLAUS",
    "SLEIGH BELLS RING",
    "DECK THE HALLS",
};
#define NUM_MESSAGES    6

// Constantly redraw the screen
static THD_WORKING_AREA(waScreenRefresh, 256);
THD_FUNCTION(screen_refresh, arg){
    (void)arg;
    chRegSetThreadName("Screen refresh");
    while(TRUE){
        display_show();
    }
}

// Randomly turn on and off the edge LEDs
static THD_WORKING_AREA(waLEDS, 128);
THD_FUNCTION(leds_update, arg){
    (void)arg;
    chRegSetThreadName("LEDs");
    while(TRUE){
        int led = rand() % 8;
        if(rand() & 1){
            led_turn_on(led);
        }else{
            led_turn_off(led);
        }
        chThdSleepMilliseconds(250 + (rand() % 512));
    }
}

// Write some text on the screen
static THD_WORKING_AREA(waText, 256);
THD_FUNCTION(text_update, arg){
    (void)arg;
    chRegSetThreadName("Text");
    
    uint32_t mode = 0;
    uint32_t delay = 60;
    uint8_t messagenum = 0;
    uint8_t animnum = 0;
    
    display_scroll_text(messages[messagenum]);
    display_set_anim(animnum);
    
    while(TRUE){
        switch(mode){
            case 0: // Scroll text
                delay = 60;
                if(display_scroll()){
                    display_scroll_text(""); // Blank the display
                    mode = 3;
                }
                break;
            case 1: // Wait then choose another message
                delay = 3000;
                messagenum += 1;
                if(messagenum >= NUM_MESSAGES){
                    messagenum = 0;
                }
                display_scroll_text(messages[messagenum]);
                mode = 0;
                break;
            case 2: // Display an animation
                delay = 100;
                if(display_anim()){
                    display_clear();
                    mode = 1;
                }
                break;
            case 3: // Wait then choose another animation
                animnum += 1;
                delay = 3000;
                if(animnum >= NUM_ANIMS){
                    animnum = 0;
                }
                display_set_anim(animnum);
                mode = 2;
                break;
        }
        chThdSleepMilliseconds(delay);
    }
}

int main(void) {
    /* Allow debug access during STOP/STANDBY */
    DBGMCU->CR |= DBGMCU_CR_DBG_STOP;

    halInit();
    // Remap the USB pins to the physical pins
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;
    chSysInit();

    display_init();

    //TODO: seed the RNG using a few ADC reads?

    chThdCreateStatic(waScreenRefresh, sizeof(waScreenRefresh), NORMALPRIO,
                    screen_refresh, NULL);
    chThdCreateStatic(waLEDS, sizeof(waLEDS), NORMALPRIO,
                    leds_update, NULL);
    chThdCreateStatic(waText, sizeof(waText), NORMALPRIO,
                    text_update, NULL);

    // Allow idle sleep to take over
    chThdSleep(TIME_INFINITE);

    return 0;
}

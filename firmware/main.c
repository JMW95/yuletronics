#include "ch.h"
#include "hal.h"

#include "rand.h"
#include "display.h"

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
    uint32_t counter = 0;
    uint8_t messagenum = 0;
    
    display_scroll_text(messages[0]);
    
    while(TRUE){
        switch(mode){
            case 0: // Scroll text
                if(display_update()){
                    display_scroll_text(""); // Blank the display
                    mode += 1;
                    counter = 0;
                }
                break;
            case 1: // Wait then choose another message
                display_update();
                counter += 1;
                if(counter == 50){ // Wait 3 seconds
                    messagenum += 1;
                    if(messagenum >= NUM_MESSAGES){
                        messagenum = 0;
                    }
                    display_scroll_text(messages[messagenum]);
                    mode = 0;
                }
                break;
        }
        chThdSleepMilliseconds(60);
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

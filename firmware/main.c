#include "ch.h"
#include "hal.h"

#include "rand.h"
#include "display.h"

// Constantly redraw the screen
static THD_WORKING_AREA(waScreenRefresh, 256);
THD_FUNCTION(screen_refresh, arg){
    (void)arg;
    chRegSetThreadName("Screen refresh");
    while(TRUE){
        display_update();
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

static const char message[] = {'M', 'e', 'r', 'r', 'y', ' ', 'C', 'h', 'r', 'i', 's', 't', 'm', 'a', 's'};

// Write some text on the screen
static THD_WORKING_AREA(waText, 256);
THD_FUNCTION(text_update, arg){
    (void)arg;
    chRegSetThreadName("Text");
    uint32_t i = 0;
    while(TRUE){
        screen_show_char(message[i]);
        i += 1;
        if(i >= sizeof(message)){
            i = 0;
        }
        chThdSleepMilliseconds(500);
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

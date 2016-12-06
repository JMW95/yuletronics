#include "ch.h"
#include "hal.h"

#include "rand.h"
#include "display.h"
#include "animations.h"
#include "easter_egg.h"
#include "usb.h"

static bool easterEggMode = false;
static uint32_t mode = 0;

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
        if(serusbcfg.usbp->state == USB_ACTIVE){
            led_turn_on(1);
        }else{
            //int led = rand() % 8;
            //if(rand() & 1){
            //    led_turn_on(led);
            //}else{
            //    led_turn_off(led);
            //}
            led_turn_off(1);
        }
        chThdSleepMilliseconds(250 + (rand() % 512));
    }
}

// Launch the easter egg if the button is held
static THD_WORKING_AREA(waEasterEgg, 256);
THD_FUNCTION(easter_egg, arg){
    (void)arg;
    chRegSetThreadName("Easter Egg");
    int heldCounter = 0;
    while(TRUE){
        if(palReadLine(LINE_SWITCH)){
            heldCounter++;
            if(heldCounter == 20){ // If held for 2 seconds
                easterEggMode = true;

                easter_egg_start(); // Run the easter egg

                mode = 3; // Play an animation
                display_clear();
                display_scroll_text("");

                easterEggMode = false;
            }
        }else{
            heldCounter = 0;
        }
        chThdSleep(100);
    }
}

// Write some text on the screen
static THD_WORKING_AREA(waText, 256);
THD_FUNCTION(text_update, arg){
    (void)arg;
    chRegSetThreadName("Modes");
    
    uint32_t delay = 60;
    uint8_t messagenum = 0;
    uint8_t animnum = 0;
    
    display_scroll_text(messages[messagenum]);
    display_set_anim(animnum);
    
    while(TRUE){
        if(!easterEggMode){
            switch(mode){
                case 0: // Scroll text
                    delay = 80;
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
        }else{
            chThdSleepMilliseconds(1000);
        }
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
    chThdCreateStatic(waEasterEgg, sizeof(waEasterEgg), NORMALPRIO,
                    easter_egg, NULL);

    sduObjectInit(&SDU1);
    
    sduStart(&SDU1, &serusbcfg);

    /*
    * Activates the USB driver and then the USB bus pull-up on D+.
    * Note, a delay is inserted in order to not have to disconnect the cable
    * after a reset.
    */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    // Allow idle sleep to take over
    chThdSleep(TIME_INFINITE);

    return 0;
}

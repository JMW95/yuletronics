#include "ch.h"
#include "hal.h"

int main(void) {
    /* Allow debug access during STOP/STANDBY */
    DBGMCU->CR |= DBGMCU_CR_DBG_STOP;

    halInit();
    chSysInit();

    while (true) {

        chThdSleepMilliseconds(100);
    }
}

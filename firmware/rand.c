#include "ch.h"
#include "rand.h"

uint32_t next = 1;

/* K&R rand function : return pseudo-random integer on 0..32767 */
uint16_t rand(void){
    next = next * 1103515245 + 12345;
    return (uint16_t)(next/65536) % 32768;
}

/* srand: set seed for rand() */
void srand(uint16_t seed) {
    next = seed;
}

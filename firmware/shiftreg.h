#ifndef SHIFTREG_H
#define SHIFTREG_H

#include "ch.h"

void shiftreg_init(void);
void shiftreg_send(uint32_t databits, uint32_t nbits);

#endif /* SHIFTREG_H */

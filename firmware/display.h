#ifndef DISPLAY_H
#define DISPLAY_H

#include "ch.h"

// LED numbers as bits in the shift register
#define LED1    0
#define LED2    5
#define LED3    6
#define LED4    7
#define LED5    4
#define LED6    1
#define LED7    2
#define LED8    3

void display_init(void);
void display_scroll_text(const char *str);
void display_set_anim(uint8_t idx);
bool display_scroll(void);
bool display_anim(void);
void display_clear(void);
void display_copy(uint8_t data[]);
void display_show(void);

void led_turn_on(int which);
void led_turn_off(int which);

#endif /* DISPLAY_H */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <stdbool.h>
#include "pico/stdlib.h"

void setup_button_interrupts(void);
void button_callback(uint gpio, uint32_t events);

#endif // BUTTON_HANDLER_H
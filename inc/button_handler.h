#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "pico/stdlib.h"

// Declaração de funções
void button_callback(uint gpio, uint32_t events);
void setup_button_interrupts(void);

#endif // BUTTON_HANDLER_H

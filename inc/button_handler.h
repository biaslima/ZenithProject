
#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <stdbool.h>
#include "pico/stdlib.h"

// Declaração da variável global para interrupção de rotina
extern volatile bool interrupt_routine;

void setup_button_interrupts(void);
void button_callback(uint gpio, uint32_t events);

#endif
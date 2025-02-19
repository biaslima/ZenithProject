#ifndef PERIFERICO_SETUP_H
#define PERIFERICO_SETUP_H

#include "config.h"
#include "pico/stdlib.h"

void gpio_setup(void);
void pwm_setup(void);
void adc_setup(void);
void display_setup(void);
void matrix_setup(void);
void setup_peripherals(void);

#endif // PERIFERICO_SETUP_H

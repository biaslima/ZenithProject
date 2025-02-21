#ifndef LED_ANIMATION_H
#define LED_ANIMATION_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "breathing_routines.h"
#include "hardware/pio.h"  // Adicione este include

extern const bool circle_patterns[3][MATRIX_SIZE][MATRIX_SIZE];
extern const bool square_patterns[3][MATRIX_SIZE][MATRIX_SIZE];

void update_led_animation(uint8_t intensity, bool breathing_in);
void set_main_led_brightness(uint8_t brightness);
void clear_all_leds(void);

// Funções de animação para respiração calmante
void animate_calm_breath_in(uint32_t duration);
void animate_calm_breath_out(uint32_t duration);
void animate_calm_breath_hold(uint32_t duration, uint8_t base_intensity);

#endif
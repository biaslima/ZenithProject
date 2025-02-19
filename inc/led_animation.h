#ifndef LED_ANIMATION_H
#define LED_ANIMATION_H

#include <stdint.h>
#include <stdbool.h>

// Definições para controle dos LEDs
#define LED_MAX_BRIGHTNESS 255
#define MATRIX_SIZE 5
#define LED_BRIGHTNESS 64  // Intensidade reduzida para conforto visual (25% do máximo)
#define BLUE_COLOR 0x000040  // Azul suave (R:0, G:0, B:64)

// Funções de controle de LEDs
void set_main_led_brightness(uint8_t brightness);
void update_led_animation(uint8_t intensity, bool breathing_in);
void clear_all_leds(void);

#endif // LED_ANIMATION_H
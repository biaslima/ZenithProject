#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Definições de pinos
#define LED_PIN_RED 13
#define LED_PIN_BLUE 12
#define LED_PIN_GREEN 11
#define BTN_PIN_A 5
#define BTN_PIN_B 6
#define MIC_PIN 28

// Outras definições...
// Pinos e variáveis display
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Pinos e Variáveis da Matriz de LEDs
#define LED_MATRIX_PIN 7
#define IS_RGBW false

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define MAX_CHARS_PER_LINE 16
#define CHAR_HEIGHT 8
#define CHAR_WIDTH 8

// LED
#define LED_MAX_BRIGHTNESS 255
#define MATRIX_SIZE 5
#define NUM_LEDS (MATRIX_SIZE * MATRIX_SIZE)

// Variáveis globais
// Em config.h
extern volatile int current_level;
extern volatile bool btn_a_ativo;
extern volatile bool btn_b_ativo;
extern volatile uint16_t ambient_noise_level;
extern volatile uint32_t last_interrupt_time_A;
extern volatile uint32_t last_interrupt_time_B;

#endif // CONFIG_H


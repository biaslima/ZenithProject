#include "led_animation.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "config.h"

// Array para controle da matriz de LEDs
static uint32_t led_matrix[NUM_LEDS];

// Define os padrões de animação para diferentes tamanhos de círculo
static const bool circle_patterns[3][MATRIX_SIZE][MATRIX_SIZE] = {
    // Círculo pequeno (centro)
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0}
    },
    // Círculo médio
    {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0}
    },
    // Círculo grande
    {
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0}
    }
};

// Função para atualizar a matriz com um padrão específico
static void update_matrix_pattern(const bool pattern[MATRIX_SIZE][MATRIX_SIZE], uint8_t intensity) {
    uint32_t color = ((uint32_t)intensity << 16) | ((uint32_t)intensity << 8) | intensity;
    
    for (int row = 0; row < MATRIX_SIZE; row++) {
        for (int col = 0; col < MATRIX_SIZE; col++) {
            int index = row * MATRIX_SIZE + col;
            led_matrix[index] = pattern[row][col] ? color : 0;
        }
    }
    
    // Atualiza a matriz física de LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio0, 0, led_matrix[i]);
    }
}

void set_main_led_brightness(uint8_t brightness) {
    pwm_set_gpio_level(LED_PIN_RED, 0);
    pwm_set_gpio_level(LED_PIN_BLUE, brightness);
    pwm_set_gpio_level(LED_PIN_GREEN, brightness);
}

void update_led_animation(uint8_t intensity, bool breathing_in) {
    // Define qual padrão usar baseado na intensidade
    int pattern_index;
    if (intensity < 85) {
        pattern_index = 0;
    } else if (intensity < 170) {
        pattern_index = 1;
    } else {
        pattern_index = 2;
    }
    
    // Calcula a intensidade proporcional para os LEDs
    uint8_t led_intensity = (intensity * LED_BRIGHTNESS) / 255;
    
    // Atualiza o padrão da matriz
    update_matrix_pattern(circle_patterns[pattern_index], led_intensity);
    
    // Atualiza o LED principal
    set_main_led_brightness(led_intensity);
}

void clear_all_leds(void) {
    set_main_led_brightness(0);
    for (int i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio0, 0, 0);
    }
}
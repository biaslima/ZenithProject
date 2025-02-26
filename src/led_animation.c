#include "led_animation.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "config.h"

#define LED_MATRIX_MAX_INTENSITY 10  // Intensidade reduzida para matriz

// Array para controle da matriz de LEDs
static uint32_t led_matrix[NUM_LEDS];

// Função para configurar o LED principal com cores específicas para cada tipo de respiração
void set_main_led_for_breathing_type(breathing_type_t type) {
    uint8_t intensity = 64;  
    
    switch (type) {
        case BREATHING_DIAPHRAGMATIC:
            // Azul esverdeado para respiração diafragmática
            pwm_set_gpio_level(LED_PIN_RED, 0);
            pwm_set_gpio_level(LED_PIN_GREEN, intensity);
            pwm_set_gpio_level(LED_PIN_BLUE, intensity + 20);  
            break;
            
        case BREATHING_SQUARE:
            // Amarelo esverdeado para respiração quadrada
            pwm_set_gpio_level(LED_PIN_RED, intensity);
            pwm_set_gpio_level(LED_PIN_GREEN, intensity);
            pwm_set_gpio_level(LED_PIN_BLUE, 0);
            break;
            
        case BREATHING_CALM:
            // Verde azulado para respiração 4-7-8
            pwm_set_gpio_level(LED_PIN_RED, 0);
            pwm_set_gpio_level(LED_PIN_GREEN, intensity + 20);  
            pwm_set_gpio_level(LED_PIN_BLUE, intensity);
            break;
            
        default:
            // Branco suave para qualquer outro caso
            pwm_set_gpio_level(LED_PIN_RED, intensity/2);
            pwm_set_gpio_level(LED_PIN_GREEN, intensity/2);
            pwm_set_gpio_level(LED_PIN_BLUE, intensity/2);
            break;
    }
}

// Define os padrões de animação para diferentes tamanhos de círculo
const bool circle_patterns[3][MATRIX_SIZE][MATRIX_SIZE] = {
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

const bool square_patterns[3][MATRIX_SIZE][MATRIX_SIZE] = {
    // Quadrado pequeno
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    },
    // Quadrado médio
    {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0}
    },
    // Quadrado grande
    {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    }
};

const bool calm_patterns[5][MATRIX_SIZE][MATRIX_SIZE] = {
    // Estágio 1 - Inicial (apenas centro)
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    },
    // Estágio 2 - Expansão suave (padrão de diamante)
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0}
    },
    // Estágio 3 - Expansão média (forma fluida)
    {
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0}
    },
    // Estágio 4 - Expansão maior (padrão de onda)
    {
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0}
    },
    // Estágio 5 - Expansão completa (quase completa)
    {
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1}
    }
};

// Função auxiliar para atualizar a matriz
static void update_matrix_pattern(const bool pattern[MATRIX_SIZE][MATRIX_SIZE], uint8_t intensity) {
    uint32_t color;
    
    if (current_breathing_type == BREATHING_SQUARE) {
        // Cor laranja mais intensa para respiração quadrada
        color = ((uint32_t)intensity << 16) |          // R (vermelho completo)
                ((uint32_t)(intensity/3) << 8);        // G (1/3 do verde para laranja)
                                                      // B = 0
    } else {
        // Mantém a cor azul/verde para respiração diafragmática
        color = ((uint32_t)(intensity) << 8) |         // G
                ((uint32_t)intensity);                  // B
                                                       // R = 0
    }

    for (int row = 0; row < MATRIX_SIZE; row++) {
        for (int col = 0; col < MATRIX_SIZE; col++) {
            int index = row * MATRIX_SIZE + col;
            led_matrix[index] = pattern[row][col] ? color : 0;
        }
    }
    
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
    int pattern_index;
    
    // Determina o padrão da matriz baseado na intensidade
    if (current_breathing_type == BREATHING_CALM) {
        // Mapeamento mais granular para a respiração calmante (5 estágios)
        if (intensity < 51) {
            pattern_index = 0;
        } else if (intensity < 102) {
            pattern_index = 1;
        } else if (intensity < 153) {
            pattern_index = 2;
        } else if (intensity < 204) {
            pattern_index = 3;
        } else {
            pattern_index = 4;
        }
        
        // O LED principal mantém intensidade constante (já configurado)
        uint8_t matrix_intensity = (intensity * LED_MATRIX_MAX_INTENSITY) / 255;
        update_matrix_pattern(calm_patterns[pattern_index], matrix_intensity);
    }
    else if (current_breathing_type == BREATHING_SQUARE) {
        if (intensity < 85) {
            pattern_index = 0;
        } else if (intensity < 170) {
            pattern_index = 1;
        } else {
            pattern_index = 2;
        }
        
        // O LED principal mantém intensidade constante (já configurado)
        uint8_t matrix_intensity = (intensity * LED_MATRIX_MAX_INTENSITY) / 255;
        update_matrix_pattern(square_patterns[pattern_index], matrix_intensity);
    }
    else { // BREATHING_DIAPHRAGMATIC
        if (intensity < 85) {
            pattern_index = 0;
        } else if (intensity < 170) {
            pattern_index = 1;
        } else {
            pattern_index = 2;
        }
        
        // O LED principal mantém intensidade constante (já configurado)
        uint8_t matrix_intensity = (intensity * LED_MATRIX_MAX_INTENSITY) / 255;
        update_matrix_pattern(circle_patterns[pattern_index], matrix_intensity);
    }
}
// Funções auxiliares de animação específicas para calm_breathing

// Animação especial fluida para inspiração
void animate_calm_breath_in(uint32_t duration) {
    // Maior número de passos para transição mais suave
    const int steps = 100;
    
    for (int i = 0; i <= steps; i++) {
        uint8_t intensity = (i * LED_MAX_BRIGHTNESS) / steps;
        update_led_animation(intensity, true);
        
        // Intervalo variável - começa mais rápido e desacelera
        uint16_t delay = (duration / steps) * (1 + (i / (float)steps) * 0.5);
        sleep_ms(delay);
    }
}

// Animação especial fluida para expiração
void animate_calm_breath_out(uint32_t duration) {
    // Maior número de passos para transição mais suave
    const int steps = 100;
    
    for (int i = steps; i >= 0; i--) {
        uint8_t intensity = (i * LED_MAX_BRIGHTNESS) / steps;
        update_led_animation(intensity, false);
        
        // Intervalo variável - começa mais devagar e acelera levemente
        uint16_t delay = (duration / steps) * (1 + ((steps - i) / (float)steps) * 0.2);
        sleep_ms(delay);
    }
}

// Animação especial para retenção respiratória (efeito ondulante)
void animate_calm_breath_hold(uint32_t duration, uint8_t base_intensity) {
    // Número de ondulações durante o período de retenção
    const int num_waves = 7;  // Um para cada segundo
    const int steps_per_wave = 20;
    
    for (int wave = 0; wave < num_waves; wave++) {
        // Onda suave para cima
        for (int i = 0; i < steps_per_wave/2; i++) {
            // Pequena variação de intensidade (apenas 10%)
            uint8_t intensity = base_intensity - (base_intensity * 0.1 * i / (steps_per_wave/2));
            update_led_animation(intensity, false);
            sleep_ms(duration / (num_waves * steps_per_wave));
        }
        
        // Onda suave para baixo
        for (int i = steps_per_wave/2; i > 0; i--) {
            // Pequena variação de intensidade (apenas 10%)
            uint8_t intensity = base_intensity - (base_intensity * 0.1 * i / (steps_per_wave/2));
            update_led_animation(intensity, false);
            sleep_ms(duration / (num_waves * steps_per_wave));
        }
    }
}

void clear_all_leds(void) {
    pwm_set_gpio_level(LED_PIN_RED, 0);
    pwm_set_gpio_level(LED_PIN_GREEN, 0);
    pwm_set_gpio_level(LED_PIN_BLUE, 0);
    
    for (int i = 0; i < NUM_LEDS; i++) {
        led_matrix[i] = 0;
        pio_sm_put_blocking(pio0, 0, 0);
    }
}
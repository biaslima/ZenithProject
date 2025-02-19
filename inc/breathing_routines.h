#ifndef BREATHING_ROUTINES_H
#define BREATHING_ROUTINES_H

#include "ssd1306.h"
#include <stdbool.h>
#include <stdint.h>

// Estrutura para definir parâmetros de respiração
typedef struct {
    uint32_t inspire_time;    // Tempo de inspiração em ms
    uint32_t expire_time;     // Tempo de expiração em ms
    uint8_t cycles;           // Número de ciclos
    const char* name;         // Nome da técnica
    uint32_t led_color;       // Cor para a matriz de LEDs
} breathing_params_t;

// Funções para rotinas de respiração
void breathing_routine(ssd1306_t *ssd, const breathing_params_t *params);
void diaphragmatic_breathing(ssd1306_t *ssd);  
void square_breathing(ssd1306_t *ssd);
#endif // BREATHING_ROUTINES_H
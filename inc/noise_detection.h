#ifndef NOISE_DETECTION_H
#define NOISE_DETECTION_H

#include <stdbool.h>
#include "ssd1306.h"

// Função para detectar níveis altos de ruído no ambiente
bool detect_loud_noise(void);

// Função para exibir o aviso de ambiente barulhento
void display_noise_warning(ssd1306_t *ssd);

// Função para verificar o ambiente antes de iniciar uma rotina
bool check_environment_before_routine(ssd1306_t *ssd);

#endif // NOISE_DETECTION_H
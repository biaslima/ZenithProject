#include <string.h>
#include <stdio.h>
#include <math.h>
#include "breathing_routines.h"
#include "led_animation.h"
#include "pico/stdlib.h"
#include "config.h"
#include "display_utils.h"
#include "noise_detection.h"

breathing_type_t current_breathing_type = BREATHING_DIAPHRAGMATIC;

// Função genérica para respiração com contagem durante a animação
static void breathing_routine(ssd1306_t *ssd, const breathing_params_t *params) {
    // Reinicia a flag de interrupção no início da rotina
    interrupt_routine = false;
    
    // Mostra título inicial
    const char *title_parts[2];
    title_parts[0] = "RESPIRACAO";
    
    // Determina qual o título baseado no tipo de respiração
    if (params->type == BREATHING_DIAPHRAGMATIC) {
        title_parts[1] = "DIAFRAGMATICA";
    } else if (params->type == BREATHING_SQUARE) {
        title_parts[1] = "QUADRADA";
    } else if (params->type == BREATHING_CALM) {
        title_parts[1] = "4-7-8";
    }
    
    display_two_lines(ssd, title_parts[0], title_parts[1]);
    sleep_ms(3000);
    
    // Verifica se houve interrupção durante a exibição do título
    if (interrupt_routine) {
        display_multiline_text(ssd, "SESSAO\nCANCELADA");
        sleep_ms(2000);
        clear_all_leds();
        return;
    }
    
    // Configura o LED principal com a cor adequada e mantém constante
    set_main_led_for_breathing_type(params->type);
    
    // Executa os ciclos de respiração
    for (int cycle = 0; cycle < params->cycles; cycle++) {
        // Verifica se houve interrupção antes de cada ciclo
        if (interrupt_routine) {
            break;
        }
        
        if (params->type == BREATHING_DIAPHRAGMATIC) {
            // Respiração Diafragmática
            display_multiline_text(ssd, "INSPIRE");
            int steps = LED_MAX_BRIGHTNESS;
            int time_per_step = params->inspire_time / steps;
            
            for (int i = 0; i <= steps; i++) {
                update_led_animation(i, true);
                int remaining = params->inspire_time / 1000 - (i * time_per_step / 1000);
                if (i % 10 == 0 && remaining >= 0) {
                    char display_text[32];
                    snprintf(display_text, sizeof(display_text), "INSPIRE\n%d", remaining);
                    display_multiline_text(ssd, display_text);
                }
                sleep_ms(time_per_step);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
            display_multiline_text(ssd, "EXPIRE");
            steps = LED_MAX_BRIGHTNESS;
            time_per_step = params->expire_time / steps;
            
            for (int i = steps; i >= 0; i--) {
                update_led_animation(i, false);
                int remaining = params->expire_time / 1000 - ((steps - i) * time_per_step / 1000);
                if (i % 10 == 0 && remaining >= 0) {
                    char display_text[32];
                    snprintf(display_text, sizeof(display_text), "EXPIRE\n%d", remaining);
                    display_multiline_text(ssd, display_text);
                }
                sleep_ms(time_per_step);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
        } else if (params->type == BREATHING_SQUARE) {
            // Respiração Quadrada
            display_multiline_text(ssd, "INSPIRE");
            int steps = LED_MAX_BRIGHTNESS;
            int time_per_step = params->inspire_time / steps;
            
            for (int i = 0; i <= steps; i++) {
                update_led_animation(i, true);
                int remaining = params->inspire_time / 1000 - (i * time_per_step / 1000);
                if (i % 10 == 0 && remaining >= 0) {
                    char display_text[32];
                    snprintf(display_text, sizeof(display_text), "INSPIRE\n%d", remaining);
                    display_multiline_text(ssd, display_text);
                }
                sleep_ms(time_per_step);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
            // Retenção com contagem
            int hold_seconds = params->hold_time / 1000;
            for (int remaining = hold_seconds; remaining > 0; remaining--) {
                char display_text[32];
                snprintf(display_text, sizeof(display_text), "SEGURE\n%d", remaining);
                display_multiline_text(ssd, display_text);
                sleep_ms(1000);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
            // Expiração com contagem durante a animação
            display_multiline_text(ssd, "EXPIRE");
            steps = LED_MAX_BRIGHTNESS;
            time_per_step = params->expire_time / steps;
            
            for (int i = steps; i >= 0; i--) {
                update_led_animation(i, false);
                int remaining = params->expire_time / 1000 - ((steps - i) * time_per_step / 1000);
                if (i % 10 == 0 && remaining >= 0) {
                    char display_text[32];
                    snprintf(display_text, sizeof(display_text), "EXPIRE\n%d", remaining);
                    display_multiline_text(ssd, display_text);
                }
                sleep_ms(time_per_step);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
            // Retenção vazio com contagem
            hold_seconds = params->hold_time / 1000;
            for (int remaining = hold_seconds; remaining > 0; remaining--) {
                char display_text[32];
                snprintf(display_text, sizeof(display_text), "MANTENHA VAZIO\n%d", remaining);
                display_multiline_text(ssd, display_text);
                sleep_ms(1000);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
        } else if (params->type == BREATHING_CALM) {
            // Respiração 4-7-8 com animações especiais
            display_multiline_text(ssd, "INSPIRE PELO\nNARIZ");
            int inspire_seconds = params->inspire_time / 1000;
            
            const int steps = 100; // Maior número de passos para transição mais suave
            int time_per_step = params->inspire_time / steps;
            
            for (int i = 0; i <= steps; i++) {
                uint8_t intensity = (i * LED_MAX_BRIGHTNESS) / steps;
                update_led_animation(intensity, true);
                
                if (i % 10 == 0) {
                    int remaining = inspire_seconds - (i * time_per_step / 1000);
                    if (remaining >= 0) {
                        char display_text[64];
                        snprintf(display_text, sizeof(display_text), "INSPIRE PELO\nNARIZ\n%d", remaining);
                        display_multiline_text(ssd, display_text);
                    }
                }
                
                uint16_t delay = time_per_step * (1 + (i / (float)steps) * 0.5);
                sleep_ms(delay);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
            // Retenção com contagem e efeito sutil de ondulação
            display_multiline_text(ssd, "SEGURE");
            int hold_seconds = params->hold_time / 1000;
            int steps_per_second = 20;
            
            for (int remaining = hold_seconds; remaining > 0; remaining--) {
                char display_text[32];
                snprintf(display_text, sizeof(display_text), "SEGURE\n%d", remaining);
                display_multiline_text(ssd, display_text);
                
                for (int step = 0; step < steps_per_second; step++) {
                    float phase = (float)step / steps_per_second * 2 * 3.14159; // 0 a 2π
                    float variation = sin(phase) * 0.1; // -0.1 a 0.1
                    uint8_t intensity = LED_MAX_BRIGHTNESS * (0.95 + variation);
                    update_led_animation(intensity, false);
                    sleep_ms(1000 / steps_per_second);
                    
                    if (interrupt_routine) {
                        goto routine_interrupted;
                    }
                }
            }
            
            // Expiração com contagem durante a animação
            display_multiline_text(ssd, "EXPIRE LENTAMENTE\nPELA BOCA");
            int expire_seconds = params->expire_time / 1000;
            time_per_step = params->expire_time / steps;
            
            for (int i = steps; i >= 0; i--) {
                uint8_t intensity = (i * LED_MAX_BRIGHTNESS) / steps;
                update_led_animation(intensity, false);
                
                if (i % 10 == 0) {
                    int remaining = expire_seconds - ((steps - i) * time_per_step / 1000);
                    if (remaining >= 0) {
                        char display_text[64];
                        snprintf(display_text, sizeof(display_text), "EXPIRE LENTAMENTE\nPELA BOCA\n%d", remaining);
                        display_multiline_text(ssd, display_text);
                    }
                }
                
                uint16_t delay = time_per_step * (1 + ((steps - i) / (float)steps) * 0.2);
                sleep_ms(delay);
                
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
        }
    }
    
    // Finalização
    display_multiline_text(ssd, "SESSAO\nCONCLUIDA");
    sleep_ms(2000);
    clear_all_leds();
    return;
    
routine_interrupted:
    // Código para tratamento da interrupção
    display_multiline_text(ssd, "SESSAO\nCANCELADA");
    sleep_ms(2000);
    clear_all_leds();
}

// Funções públicas para as rotinas de respiração
void diaphragmatic_breathing(ssd1306_t *ssd) {
    // Verifica se o ambiente está adequado primeiro
    if (!check_environment_before_routine(ssd)) {
        return;
    }
    
    current_breathing_type = BREATHING_DIAPHRAGMATIC;
    breathing_params_t params = {
        .type = BREATHING_DIAPHRAGMATIC,
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 6000,     // 6 segundos
        .hold_time = 0,          // Sem pausa
        .cycles = 5,             // 5 ciclos
    };
    
    breathing_routine(ssd, &params);
}

void square_breathing(ssd1306_t *ssd) {
    // Verifica se o ambiente está adequado primeiro
    if (!check_environment_before_routine(ssd)) {
        return;
    }
    
    current_breathing_type = BREATHING_SQUARE;
    breathing_params_t params = {
        .type = BREATHING_SQUARE,
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 4000,     // 4 segundos
        .hold_time = 4000,       // 4 segundos
        .cycles = 9,             // 9 ciclos
    };
    
    breathing_routine(ssd, &params);
}

void calm_breathing(ssd1306_t *ssd) {
    // Verifica se o ambiente está adequado primeiro
    if (!check_environment_before_routine(ssd)) {
        return;
    }
    
    current_breathing_type = BREATHING_CALM;
    breathing_params_t params = {
        .type = BREATHING_CALM,
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 8000,     // 8 segundos
        .hold_time = 7000,       // 7 segundos
        .cycles = 4,             // 4 ciclos
    };
    
    breathing_routine(ssd, &params);
}
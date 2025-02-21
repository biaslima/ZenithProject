#include <string.h>
#include <stdio.h>
#include <math.h>
#include "breathing_routines.h"
#include "led_animation.h"
#include "pico/stdlib.h"
#include "config.h"

breathing_type_t current_breathing_type = BREATHING_DIAPHRAGMATIC;

// Referência à variável global definida em button_handler.c
extern volatile bool interrupt_routine;

// Declarações de funções estáticas (internas)
static void display_multiline_text(ssd1306_t *ssd, const char* text);
static void breathing_routine(ssd1306_t *ssd, const breathing_params_t *params);
static void write_centered_text(ssd1306_t *ssd, const char* text, int y);
static void display_two_lines(ssd1306_t *ssd, const char* line1, const char* line2);
static void wrap_text(const char* text, char* wrapped_text, int max_width);
static void display_wrapped_message(ssd1306_t *ssd, const char* text);
static void animate_breath_in(uint32_t duration);
static void animate_breath_out(uint32_t duration);

// Função para verificar o ambiente antes de iniciar uma rotina de respiração
extern bool check_environment_before_routine(ssd1306_t *ssd);

// Funções auxiliares para o display
static void write_centered_text(ssd1306_t *ssd, const char* text, int y) {
    int width = strlen(text) * 8;
    int x = (DISPLAY_WIDTH - width) / 2;
    ssd1306_draw_string(ssd, text, x, y);
}

static void display_two_lines(ssd1306_t *ssd, const char* line1, const char* line2) {
    ssd1306_fill(ssd, false);
    
    int y1 = (DISPLAY_HEIGHT / 2) - 12;  // 12 pixels acima do centro
    int y2 = (DISPLAY_HEIGHT / 2) + 4;   // 4 pixels abaixo do centro
    
    write_centered_text(ssd, line1, y1);
    write_centered_text(ssd, line2, y2);
    ssd1306_send_data(ssd);
}

static void wrap_text(const char* text, char* wrapped_text, int max_width) {
    int text_len = strlen(text);
    int current_line_len = 0;
    int wrapped_index = 0;
    
    for (int i = 0; i < text_len; i++) {
        if (current_line_len >= max_width && text[i] == ' ') {
            wrapped_text[wrapped_index++] = '\n';
            current_line_len = 0;
        } else {
            wrapped_text[wrapped_index++] = text[i];
            current_line_len++;
            
            // Força quebra se palavra é maior que largura máxima
            if (current_line_len >= max_width && text[i] != ' ' && 
                (i + 1 < text_len && text[i + 1] != ' ')) {
                wrapped_text[wrapped_index++] = '\n';
                current_line_len = 0;
            }
        }
    }
    wrapped_text[wrapped_index] = '\0';
}

static void display_wrapped_message(ssd1306_t *ssd, const char* text) {
    char wrapped_text[256];
    wrap_text(text, wrapped_text, MAX_CHARS_PER_LINE);
    
    ssd1306_fill(ssd, false);
    
    // Primeiro, conte quantas linhas teremos
    int num_lines = 1;  // Começa com 1 linha
    for(int i = 0; wrapped_text[i]; i++) {
        if(wrapped_text[i] == '\n') num_lines++;
    }
    
    // Calcula a posição y inicial para centralizar verticalmente
    int total_height = num_lines * (CHAR_HEIGHT + 2);  // altura total do texto
    int start_y = (DISPLAY_HEIGHT - total_height) / 2;  // posição y inicial
    
    // Desenha cada linha
    char* line = strtok(wrapped_text, "\n");
    int current_y = start_y;
    
    while (line != NULL) {
        int width = strlen(line) * CHAR_WIDTH;
        int x = (DISPLAY_WIDTH - width) / 2;  // centraliza horizontalmente
        if(x < 0) x = 0;
        
        ssd1306_draw_string(ssd, line, x, current_y);
        current_y += CHAR_HEIGHT + 2;
        line = strtok(NULL, "\n");
    }
    
    ssd1306_send_data(ssd);
}

// Versão melhorada para exibir texto com múltiplas linhas
static void display_multiline_text(ssd1306_t *ssd, const char* text) {
    ssd1306_fill(ssd, false);
    
    // Procura por quebras de linha
    const char* start = text;
    const char* newline;
    int line_count = 0;
    int lines_total = 0;
    
    // Conta o número total de linhas
    const char* count_ptr = text;
    while (*count_ptr) {
        if (*count_ptr == '\n') lines_total++;
        count_ptr++;
    }
    lines_total++; // Adiciona a última linha
    
    // Calcula a altura total e posição inicial
    int line_height = CHAR_HEIGHT + 2;
    int total_height = lines_total * line_height;
    int start_y = (DISPLAY_HEIGHT - total_height) / 2;
    if (start_y < 0) start_y = 0;
    
    int current_y = start_y;
    
    while ((newline = strchr(start, '\n')) != NULL) {
        char line[32];
        int length = newline - start;
        if (length >= sizeof(line)) length = sizeof(line) - 1;
        
        strncpy(line, start, length);
        line[length] = '\0';
        
        // Centraliza horizontalmente
        int width = strlen(line) * CHAR_WIDTH;
        int x = (DISPLAY_WIDTH - width) / 2;
        if (x < 0) x = 0;
        
        ssd1306_draw_string(ssd, line, x, current_y);
        current_y += line_height;
        
        start = newline + 1;
        line_count++;
    }
    
    // Última linha ou única linha se não houver quebras
    if (*start) {
        int width = strlen(start) * CHAR_WIDTH;
        int x = (DISPLAY_WIDTH - width) / 2;
        if (x < 0) x = 0;
        
        ssd1306_draw_string(ssd, start, x, current_y);
    }
    
    ssd1306_send_data(ssd);
}

// Funções de animação LED
static void animate_breath_in(uint32_t duration) {
    for (int i = 0; i <= LED_MAX_BRIGHTNESS; i++) {
        update_led_animation(i, true);
        sleep_ms(duration / LED_MAX_BRIGHTNESS);
        
        // Verifica se houve interrupção durante a animação
        if (interrupt_routine) {
            return;
        }
    }
}

static void animate_breath_out(uint32_t duration) {
    for (int i = LED_MAX_BRIGHTNESS; i >= 0; i--) {
        update_led_animation(i, false);
        sleep_ms(duration / LED_MAX_BRIGHTNESS);
        
        // Verifica se houve interrupção durante a animação
        if (interrupt_routine) {
            return;
        }
    }
}

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
        ssd1306_fill(ssd, false);
        display_multiline_text(ssd, "SESSAO\nCANCELADA");
        sleep_ms(2000);
        clear_all_leds();
        return;
    }
    
    // Configura o LED principal com a cor adequada e mantém constante
    set_main_led_for_breathing_type((breathing_type_t)params->type);
    
    // Executa os ciclos de respiração
    for (int cycle = 0; cycle < params->cycles; cycle++) {
        // Verifica se houve interrupção antes de cada ciclo
        if (interrupt_routine) {
            break;
        }
        
        if (params->type == BREATHING_DIAPHRAGMATIC) {
            // Respiração Diafragmática
            // Inspiração com contagem durante a animação
            display_multiline_text(ssd, "INSPIRE");
            int steps = LED_MAX_BRIGHTNESS;
            int time_per_step = params->inspire_time / steps;
            
            for (int i = 0; i <= steps; i++) {
                update_led_animation(i, true);
                int remaining = params->inspire_time / 1000 - (i * time_per_step / 1000);
                if (i % 10 == 0 && remaining >= 0) { // Atualiza a cada 10 passos para não piscar muito
                    char display_text[32];
                    snprintf(display_text, sizeof(display_text), "INSPIRE\n%d", remaining);
                    display_multiline_text(ssd, display_text);
                }
                sleep_ms(time_per_step);
                
                // Verifica se houve interrupção durante a inspiração
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
                
                // Verifica se houve interrupção durante a expiração
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
        } else if (params->type == BREATHING_SQUARE) {
            // Respiração Quadrada
            // Inspiração com contagem durante a animação
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
                
                // Verifica se houve interrupção durante a inspiração
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
                
                // Verifica se houve interrupção durante a retenção
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
                
                // Verifica se houve interrupção durante a expiração
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
                
                // Verifica se houve interrupção durante a retenção vazia
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
        } else if (params->type == BREATHING_CALM) {
            // Respiração 4-7-8 com animações especiais
            // Inspiração com contagem durante a animação
            display_multiline_text(ssd, "INSPIRE PELO\nNARIZ");
            int inspire_seconds = params->inspire_time / 1000;
            
            // Usando a função animate_calm_breath_in mas com contagem integrada
            const int steps = 100; // Maior número de passos para transição mais suave
            int time_per_step = params->inspire_time / steps;
            
            for (int i = 0; i <= steps; i++) {
                uint8_t intensity = (i * LED_MAX_BRIGHTNESS) / steps;
                update_led_animation(intensity, true);
                
                // Atualiza a contagem a cada 10 passos
                if (i % 10 == 0) {
                    int remaining = inspire_seconds - (i * time_per_step / 1000);
                    if (remaining >= 0) {
                        char display_text[64];
                        snprintf(display_text, sizeof(display_text), "INSPIRE PELO\nNARIZ\n%d", remaining);
                        display_multiline_text(ssd, display_text);
                    }
                }
                
                // Intervalo variável para efeito suave
                uint16_t delay = time_per_step * (1 + (i / (float)steps) * 0.5);
                sleep_ms(delay);
                
                // Verifica se houve interrupção durante a inspiração
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
            
            // Retenção com contagem e efeito sutil de ondulação
            display_multiline_text(ssd, "SEGURE");
            int hold_seconds = params->hold_time / 1000;
            int steps_per_second = 20;
            int total_steps = hold_seconds * steps_per_second;
            
            for (int remaining = hold_seconds; remaining > 0; remaining--) {
                // Exibe a contagem atualizada
                char display_text[32];
                snprintf(display_text, sizeof(display_text), "SEGURE\n%d", remaining);
                display_multiline_text(ssd, display_text);
                
                // Realiza o efeito de ondulação durante cada segundo
                for (int step = 0; step < steps_per_second; step++) {
                    // Pequena variação de intensidade (apenas 10%)
                    float phase = (float)step / steps_per_second * 2 * 3.14159; // 0 a 2π
                    float variation = sin(phase) * 0.1; // -0.1 a 0.1
                    uint8_t intensity = LED_MAX_BRIGHTNESS * (0.95 + variation);
                    update_led_animation(intensity, false);
                    sleep_ms(1000 / steps_per_second);
                    
                    // Verifica se houve interrupção durante a ondulação
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
                
                // Atualiza a contagem a cada 10 passos
                if (i % 10 == 0) {
                    int remaining = expire_seconds - ((steps - i) * time_per_step / 1000);
                    if (remaining >= 0) {
                        char display_text[64];
                        snprintf(display_text, sizeof(display_text), "EXPIRE LENTAMENTE\nPELA BOCA\n%d", remaining);
                        display_multiline_text(ssd, display_text);
                    }
                }
                
                // Intervalo variável para efeito suave
                uint16_t delay = time_per_step * (1 + ((steps - i) / (float)steps) * 0.2);
                sleep_ms(delay);
                
                // Verifica se houve interrupção durante a expiração
                if (interrupt_routine) {
                    goto routine_interrupted;
                }
            }
        }
    }
    
    // Finalização com quebra de linha explícita
    display_multiline_text(ssd, "SESSAO\nCONCLUIDA");
    sleep_ms(2000);
    clear_all_leds();
    return;
    
routine_interrupted:
    // Código para tratamento da interrupção
    ssd1306_fill(ssd, false);
    display_multiline_text(ssd, "SESSAO\nCANCELADA");
    sleep_ms(2000);
    clear_all_leds();
}

// Funções públicas para as rotinas de respiração
void diaphragmatic_breathing(ssd1306_t *ssd) {
    // Verifica se o ambiente está adequado primeiro
    if (!check_environment_before_routine(ssd))
    
    current_breathing_type = BREATHING_DIAPHRAGMATIC;
    breathing_params_t params = {
        .type = BREATHING_DIAPHRAGMATIC,
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 6000,     // 6 segundos
        .hold_time = 0,          // Sem pausa
        .cycles = 5,             // 5 ciclos (~50 segundos)
    };
    
    breathing_routine(ssd, &params);
}

void square_breathing(ssd1306_t *ssd) {
    // Verifica se o ambiente está adequado primeiro
    if (!check_environment_before_routine(ssd)) 
    current_breathing_type = BREATHING_SQUARE;
    breathing_params_t params = {
        .type = BREATHING_SQUARE,
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 4000,     // 4 segundos
        .hold_time = 4000,       // 4 segundos
        .cycles = 9,             // 9 ciclos (~2.5 minutos)
    };
    
    breathing_routine(ssd, &params);
}

void calm_breathing(ssd1306_t *ssd) {
    // Verifica se o ambiente está adequado primeiro
    if (!check_environment_before_routine(ssd))
    
    current_breathing_type = BREATHING_CALM;
    breathing_params_t params = {
        .type = BREATHING_CALM,
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 8000,     // 8 segundos
        .hold_time = 7000,       // 7 segundos
        .cycles = 4,             // 4 ciclos inicialmente
    };
    
    breathing_routine(ssd, &params);
}
#include <string.h>
#include "breathing_routines.h"
#include "led_animation.h"
#include "pico/stdlib.h"
#include "config.h"

breathing_type_t current_breathing_type = BREATHING_DIAPHRAGMATIC;

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

// Funções de animação LED
static void animate_breath_in(uint32_t duration) {
    for (int i = 0; i <= LED_MAX_BRIGHTNESS; i++) {
        update_led_animation(i, true);
        sleep_ms(duration / LED_MAX_BRIGHTNESS);
    }
}

static void animate_breath_out(uint32_t duration) {
    for (int i = LED_MAX_BRIGHTNESS; i >= 0; i--) {
        update_led_animation(i, false);
        sleep_ms(duration / LED_MAX_BRIGHTNESS);
    }
}

// Função genérica para respiração
static void breathing_routine(ssd1306_t *ssd, const breathing_params_t *params) {
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
    
    // Acende LED central com intensidade baixa constante
    set_main_led_brightness(32);
    
    // Executa os ciclos de respiração
    for (int cycle = 0; cycle < params->cycles; cycle++) {
        if (params->type == BREATHING_DIAPHRAGMATIC) {
            // Respiração Diafragmática
            display_wrapped_message(ssd, "INSPIRE...");
            animate_breath_in(params->inspire_time);
            
            display_wrapped_message(ssd, "EXPIRE...");
            animate_breath_out(params->expire_time);
        } else if (params->type == BREATHING_SQUARE) {
            // Respiração Quadrada
            display_wrapped_message(ssd, "INSPIRE...");
            animate_breath_in(params->inspire_time);
            
            display_wrapped_message(ssd, "SEGURE...");
            sleep_ms(params->hold_time);
            
            display_wrapped_message(ssd, "EXPIRE...");
            animate_breath_out(params->expire_time);
            
            display_wrapped_message(ssd, "MANTENHA VAZIO...");
            sleep_ms(params->hold_time);
        } else if (params->type == BREATHING_CALM) {
            // Respiração 4-7-8 com animações especiais
            display_wrapped_message(ssd, "INSPIRE PELO NARIZ... (4)");
            animate_calm_breath_in(params->inspire_time);
            
            display_wrapped_message(ssd, "SEGURE... (7)");
            animate_calm_breath_hold(params->hold_time, LED_MAX_BRIGHTNESS);
            
            display_wrapped_message(ssd, "EXPIRE LENTAMENTE PELA BOCA... (8)");
            animate_calm_breath_out(params->expire_time);
        }
    }
    
    // Finalização
    display_wrapped_message(ssd, "SESSAO CONCLUIDA");
    sleep_ms(2000);
    clear_all_leds();
}
// Função para respiração diafragmática
void diaphragmatic_breathing(ssd1306_t *ssd) {
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

// Função para respiração quadrada
void square_breathing(ssd1306_t *ssd) {
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
// Nova versão da função calm_breathing 
void calm_breathing(ssd1306_t *ssd) {
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
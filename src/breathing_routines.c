#include <string.h>
#include "breathing_routines.h"
#include "led_animation.h"
#include "pico/stdlib.h"
#include "config.h"

// Função auxiliar para exibir texto em duas linhas
void display_title(ssd1306_t *ssd, const char* text) {
    ssd1306_fill(ssd, false);
    
    // Para respiração diafragmática, quebra em duas linhas
    const char* line1 = "RESPIRACAO";
    const char* line2 = "DIAFRAGMATICA";
    
    // Calcula posição central para primeira linha
    int width1 = strlen(line1) * 8;
    int x1 = (DISPLAY_WIDTH - width1) / 2;
    int y1 = (DISPLAY_HEIGHT / 2) - 12;  // 12 pixels acima do centro
    
    // Calcula posição central para segunda linha
    int width2 = strlen(line2) * 8;
    int x2 = (DISPLAY_WIDTH - width2) / 2;
    int y2 = (DISPLAY_HEIGHT / 2) + 4;   // 4 pixels abaixo do centro
    
    // Desenha as duas linhas
    ssd1306_draw_string(ssd, line1, x1, y1);
    ssd1306_draw_string(ssd, line2, x2, y2);
    ssd1306_send_data(ssd);
}

void breathing_routine(ssd1306_t *ssd, const breathing_params_t *params) {
    int x, y;
    
    // Acende o LED central com intensidade baixa constante
    set_main_led_brightness(32);  // Intensidade bem baixa (1/8 do máximo)
    
    // Exibe o título em duas linhas
    display_title(ssd, params->name);
    sleep_ms(3000);

    y = (DISPLAY_HEIGHT - 8) / 2;

    for (int cycle = 0; cycle < params->cycles; cycle++) {
        // Inspiração centralizada
        ssd1306_fill(ssd, false);
        const char* inspire_text = "INSPIRE...";
        int inspire_width = strlen(inspire_text) * 8;
        x = (DISPLAY_WIDTH - inspire_width) / 2;
        ssd1306_draw_string(ssd, inspire_text, x, y);
        ssd1306_send_data(ssd);
        
        // Anima a matriz crescendo
        for (int i = 0; i <= LED_MAX_BRIGHTNESS; i++) {
            update_led_animation(i, true);
            sleep_ms(params->inspire_time / LED_MAX_BRIGHTNESS);
        }
        
        // Expiração centralizada
        ssd1306_fill(ssd, false);
        const char* expire_text = "EXPIRE...";
        int expire_width = strlen(expire_text) * 8;
        x = (DISPLAY_WIDTH - expire_width) / 2;
        ssd1306_draw_string(ssd, expire_text, x, y);
        ssd1306_send_data(ssd);
        
        // Anima a matriz diminuindo
        for (int i = LED_MAX_BRIGHTNESS; i >= 0; i--) {
            update_led_animation(i, false);
            sleep_ms(params->expire_time / LED_MAX_BRIGHTNESS);
        }
    }
    
    // Mensagem final centralizada
    ssd1306_fill(ssd, false);
    const char* final_text = "SESSAO CONCLUIDA";
    int final_width = strlen(final_text) * 8;
    x = (DISPLAY_WIDTH - final_width) / 2;
    ssd1306_draw_string(ssd, final_text, x, y);
    ssd1306_send_data(ssd);
    sleep_ms(2000);
    
    clear_all_leds();
}

void square_breathing(ssd1306_t *ssd) {
    breathing_params_t params = {
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 4000,     // 4 segundos
        .cycles = 9,             // 9 ciclos (aprox. 2.5 minutos)
        .name = "RESPIRACAO QUADRADA",
        .led_color = BLUE_COLOR
    };

    int x, y;
    
    // Acende o LED central com intensidade baixa constante
    set_main_led_brightness(32);
    
    // Exibe o título em duas linhas
    ssd1306_fill(ssd, false);
    const char* line1 = "RESPIRACAO";
    const char* line2 = "QUADRADA";
    
    int width1 = strlen(line1) * 8;
    int width2 = strlen(line2) * 8;
    int x1 = (DISPLAY_WIDTH - width1) / 2;
    int x2 = (DISPLAY_WIDTH - width2) / 2;
    int y1 = (DISPLAY_HEIGHT / 2) - 12;
    int y2 = (DISPLAY_HEIGHT / 2) + 4;
    
    ssd1306_draw_string(ssd, line1, x1, y1);
    ssd1306_draw_string(ssd, line2, x2, y2);
    ssd1306_send_data(ssd);
    sleep_ms(3000);

    y = (DISPLAY_HEIGHT - 8) / 2;

    for (int cycle = 0; cycle < params.cycles; cycle++) {
        // 1. Inspiração (4 segundos)
        ssd1306_fill(ssd, false);
        const char* inspire_text = "INSPIRE...";
        int width = strlen(inspire_text) * 8;
        x = (DISPLAY_WIDTH - width) / 2;
        ssd1306_draw_string(ssd, inspire_text, x, y);
        ssd1306_send_data(ssd);
        
        for (int i = 0; i <= LED_MAX_BRIGHTNESS; i++) {
            update_led_animation(i, true);
            sleep_ms(params.inspire_time / LED_MAX_BRIGHTNESS);
        }
        
        // 2. Prenda a respiração (4 segundos)
        ssd1306_fill(ssd, false);
        const char* hold_text = "SEGURE...";
        width = strlen(hold_text) * 8;
        x = (DISPLAY_WIDTH - width) / 2;
        ssd1306_draw_string(ssd, hold_text, x, y);
        ssd1306_send_data(ssd);
        sleep_ms(4000);  // Mantém o último padrão da matriz por 4 segundos
        
        // 3. Expiração (4 segundos)
        ssd1306_fill(ssd, false);
        const char* expire_text = "EXPIRE...";
        width = strlen(expire_text) * 8;
        x = (DISPLAY_WIDTH - width) / 2;
        ssd1306_draw_string(ssd, expire_text, x, y);
        ssd1306_send_data(ssd);
        
        for (int i = LED_MAX_BRIGHTNESS; i >= 0; i--) {
            update_led_animation(i, false);
            sleep_ms(params.expire_time / LED_MAX_BRIGHTNESS);
        }
        
        // 4. Mantenha os pulmões vazios (4 segundos)
        ssd1306_fill(ssd, false);
        const char* empty_text = "MANTENHA VAZIO...";
        width = strlen(empty_text) * 8;
        x = (DISPLAY_WIDTH - width) / 2;
        ssd1306_draw_string(ssd, empty_text, x, y);
        ssd1306_send_data(ssd);
        sleep_ms(4000);  // Mantém o padrão vazio por 4 segundos
    }
    
    // Mensagem final
    ssd1306_fill(ssd, false);
    const char* final_text = "SESSAO CONCLUIDA";
    int final_width = strlen(final_text) * 8;
    x = (DISPLAY_WIDTH - final_width) / 2;
    ssd1306_draw_string(ssd, final_text, x, y);
    ssd1306_send_data(ssd);
    sleep_ms(2000);
    
    clear_all_leds();
}

void diaphragmatic_breathing(ssd1306_t *ssd) {
    breathing_params_t params = {
        .inspire_time = 4000,    // 4 segundos
        .expire_time = 6000,     // 6 segundos
        .cycles = 5,             // 5 ciclos (~50 segundos)
        .name = "RESPIRACAO DIAFRAGMATICA",
        .led_color = BLUE_COLOR  // Azul suave para relaxamento
    };
    
    breathing_routine(ssd, &params);
}
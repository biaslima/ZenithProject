#include "breathing_routines.h"
#include "led_animation.h"
#include "pico/stdlib.h"

void breathing_routine(ssd1306_t *ssd, const breathing_params_t *params) {
    // Exibe o nome da técnica
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, params->name, 0, 0);
    ssd1306_send_data(ssd);
    sleep_ms(3000);

    for (int cycle = 0; cycle < params->cycles; cycle++) {
        // Inspiração
        ssd1306_fill(ssd, false);
        ssd1306_draw_string(ssd, "INSPIRE...", 0, 0);
        ssd1306_send_data(ssd);
        
        for (int i = 0; i <= LED_MAX_BRIGHTNESS; i++) {
            update_led_animation(i, true);
            sleep_ms(params->inspire_time / LED_MAX_BRIGHTNESS);
        }
        
        // Expiração
        ssd1306_fill(ssd, false);
        ssd1306_draw_string(ssd, "EXPIRE...", 0, 0);
        ssd1306_send_data(ssd);
        
        for (int i = LED_MAX_BRIGHTNESS; i >= 0; i--) {
            update_led_animation(i, false);
            sleep_ms(params->expire_time / LED_MAX_BRIGHTNESS);
        }
    }
    
    // Finaliza a sessão
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "SESSAO CONCLUIDA", 0, 0);
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
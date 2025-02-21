#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "ssd1306.h"
#include "config.h"
#include "noise_detection.h"

// Função para detectar níveis altos de ruído no ambiente
bool detect_loud_noise(void) {
    uint32_t sum = 0;
    uint16_t max_value = 0;
    uint16_t sample;
    
    // Seleciona a entrada do ADC para o microfone
    adc_select_input(2);  
    
    // Coleta várias amostras do microfone
    for (int i = 0; i < MIC_SAMPLES; i++) {
        sample = adc_read();
        
        // Registra o valor máximo detectado
        if (sample > max_value) {
            max_value = sample;
        }
        
        // Soma para média
        sum += sample;
        
        // Pequena pausa entre leituras
        sleep_us(100);
    }
    
    uint16_t avg_value = sum / MIC_SAMPLES;
    
    // Retorna true se o valor máximo ultrapassar o threshold
    printf("Mic reading - Max: %d, Avg: %d, Threshold: %d\n", max_value, avg_value, MIC_THRESHOLD);
    return (max_value > MIC_THRESHOLD);
}

// Função para exibir o aviso de ambiente barulhento
void display_noise_warning(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    
    const char* line1 = "AMBIENTE";
    const char* line2 = "BARULHENTO";

    
    int line_height = CHAR_HEIGHT + 2;
    int start_y = (DISPLAY_HEIGHT - (4 * line_height)) / 2;
    
    // Centraliza e exibe cada linha
    int width1 = strlen(line1) * CHAR_WIDTH;
    int width2 = strlen(line2) * CHAR_WIDTH;
    
    int x1 = (DISPLAY_WIDTH - width1) / 2;
    int x2 = (DISPLAY_WIDTH - width2) / 2;
    
    if (x1 < 0) x1 = 0;
    if (x2 < 0) x2 = 0;
    
    ssd1306_draw_string(ssd, line1, x1, start_y);
    ssd1306_draw_string(ssd, line2, x2, start_y + line_height);
    
    ssd1306_send_data(ssd);
    
    // Vamos piscar o LED principal em vermelho como alerta
    for (int i = 0; i < 3; i++) {
        // Pisca em vermelho
        pwm_set_gpio_level(LED_PIN_RED, 200);
        pwm_set_gpio_level(LED_PIN_GREEN, 0);
        pwm_set_gpio_level(LED_PIN_BLUE, 0);
        sleep_ms(500);
        
        // Desliga
        pwm_set_gpio_level(LED_PIN_RED, 0);
        pwm_set_gpio_level(LED_PIN_GREEN, 0);
        pwm_set_gpio_level(LED_PIN_BLUE, 0);
        sleep_ms(500);
    }
    
    // Aguarda 3 segundos com a mensagem na tela
    sleep_ms(3000);
}

// Função para verificar o ambiente antes de iniciar uma rotina
bool check_environment_before_routine(ssd1306_t *ssd) {
    // Verifica se o ambiente está barulhento
    if (detect_loud_noise()) {
        display_noise_warning(ssd);
        return false;  // Ambiente não está adequado
    }
    
    return true;  // Ambiente está adequado
}
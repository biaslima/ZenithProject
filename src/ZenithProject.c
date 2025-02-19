#include <stdio.h>
#include "pico/stdlib.h"
#include "config.h"  // Removido "inc/"
#include "periferico_setup.h"  // Removido "inc/"
#include "button_handler.h"  // Removido "inc/"
#include "anxiety_level.h"  // Removido "inc/"
#include "ssd1306.h"  // Removido "inc/"
#include "ws2812.pio.h"

// Adicionar o protótipo da função write_to_display no topo do arquivo
void write_to_display(ssd1306_t *ssd, const char *text);

void write_to_display(ssd1306_t *ssd, const char *text) {
    ssd1306_fill(ssd, false);  // Limpa o display
    ssd1306_draw_string(ssd, text, 0, 0);  // Escreve o texto na posição (0,0)
    ssd1306_send_data(ssd);  // Atualiza o display
}


int main() {
    stdio_init_all();
    
    // Configuração dos periféricos
    setup_peripherals();
    
    // Configuração das interrupções dos botões
    setup_button_interrupts();  // Descomentado esta linha
    
    // Inicialização do display OLED
    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    
    while (true) {
        determine_anxiety_level();
        
        char display_text[20];
        snprintf(display_text, sizeof(display_text), "NIVEL: %d", current_level);
        write_to_display(&ssd, display_text);
        
        printf("Nivel de ansiedade: %d\n", current_level);
        sleep_ms(1000);
    }
    
    return 0;
}


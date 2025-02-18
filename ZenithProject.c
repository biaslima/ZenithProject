#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/config.h"
#include "inc/periferico_setup.h"
#include "inc/button_handler.h"
#include "inc/anxiety_level.h"
#include "inc/ssd1306.h"

int main() {
    stdio_init_all();
    
    // Configuração dos periféricos
    setup_peripherals();
    
    // Configuração das interrupções dos botões
    setup_button_interrupts();

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
        
        // Adicione aqui a lógica específica para cada nível de ansiedade...
    }

    return 0;
}

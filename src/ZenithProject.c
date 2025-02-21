#include <stdio.h>
#include "pico/stdlib.h"
#include "config.h"
#include "periferico_setup.h"
#include "button_handler.h"
#include "anxiety_level.h"
#include "ssd1306.h"
#include "breathing_routines.h"
#include "display_utils.h"

int main() {
    stdio_init_all();
    setup_peripherals();
    setup_button_interrupts();
    
    ssd1306_t ssd;
    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, I2C_DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    
    // Mostrar tela inicial
    show_splash_screen(&ssd);
    
    while (true) {
        determine_anxiety_level();
        
        switch (current_level) {
            case 1:
                diaphragmatic_breathing(&ssd);
                break;
            case 2:
                square_breathing(&ssd);
                break;
            case 3:
                calm_breathing(&ssd); 
                break;
            default:
                // Mostra título e instrução
                display_two_lines(&ssd, "ZENITH PROJECT", "ESCOLHA O MODO");
                sleep_ms(100);
                break;
        }
        
        printf("Nivel de ansiedade: %d\n", current_level);
    }
    
    return 0;
}
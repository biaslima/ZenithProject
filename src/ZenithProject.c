#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "config.h"
#include "periferico_setup.h"
#include "button_handler.h"
#include "anxiety_level.h"
#include "ssd1306.h"
#include "breathing_routines.h"
#include "ws2812.pio.h"

// Constantes para o display
#define MAX_CHARS_PER_LINE 16
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8
#define LINE_SPACING 12

// Protótipos das funções
void to_uppercase(char* str);
void write_centered_text(ssd1306_t *ssd, const char* text);
void write_multiline_text(ssd1306_t *ssd, const char* line1, const char* line2);
void show_splash_screen(ssd1306_t *ssd);

// Implementação da função para converter texto para maiúsculo
void to_uppercase(char* str) {
    for(int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Implementação da função para escrever texto centralizado
void write_centered_text(ssd1306_t *ssd, const char* text) {
    char display_text[64];
    strncpy(display_text, text, sizeof(display_text) - 1);
    display_text[sizeof(display_text) - 1] = '\0';
    
    to_uppercase(display_text);
    
    // Calcula a posição central
    int text_width = strlen(display_text) * CHAR_WIDTH;
    int x = (DISPLAY_WIDTH - text_width) / 2;
    int y = (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, display_text, x, y);
    ssd1306_send_data(ssd);
}

// Implementação da função para escrever texto em duas linhas
void write_multiline_text(ssd1306_t *ssd, const char* line1, const char* line2) {
    char display_line1[32];
    char display_line2[32];
    
    strncpy(display_line1, line1, sizeof(display_line1) - 1);
    strncpy(display_line2, line2, sizeof(display_line2) - 1);
    display_line1[sizeof(display_line1) - 1] = '\0';
    display_line2[sizeof(display_line2) - 1] = '\0';
    
    to_uppercase(display_line1);
    to_uppercase(display_line2);
    
    // Calcula as posições centrais
    int text_width1 = strlen(display_line1) * CHAR_WIDTH;
    int text_width2 = strlen(display_line2) * CHAR_WIDTH;
    
    int x1 = (DISPLAY_WIDTH - text_width1) / 2;
    int x2 = (DISPLAY_WIDTH - text_width2) / 2;
    
    // Posiciona as linhas verticalmente
    int y1 = (DISPLAY_HEIGHT - (2 * CHAR_HEIGHT + LINE_SPACING)) / 2;
    int y2 = y1 + CHAR_HEIGHT + LINE_SPACING;
    
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, display_line1, x1, y1);
    ssd1306_draw_string(ssd, display_line2, x2, y2);
    ssd1306_send_data(ssd);
}

// Implementação da função de tela inicial
void show_splash_screen(ssd1306_t *ssd) {
    write_centered_text(ssd, "ZENITH PROJECT");
    sleep_ms(2000);
}

int main() {
    stdio_init_all();
    setup_peripherals();
    setup_button_interrupts();
    
    ssd1306_t ssd;
    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, endereco, I2C_PORT);
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
                // box_breathing(&ssd);
                break;
            case 3:
                // square_breathing(&ssd);
                break;
            default:
                // Mostra título e instrução
                write_multiline_text(&ssd, "ZENITH PROJECT", "ESCOLHA O MODO");
                sleep_ms(100);
                break;
        }
        
        printf("Nivel de ansiedade: %d\n", current_level);
    }
    
    return 0;
}

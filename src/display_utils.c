#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "display_utils.h"
#include "config.h"

// Converte texto para maiúsculo
void to_uppercase(char* str) {
    for(int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Centraliza texto horizontalmente
void write_centered_text(ssd1306_t *ssd, const char* text, int y) {
    int width = strlen(text) * CHAR_WIDTH;
    int x = (DISPLAY_WIDTH - width) / 2;
    if (x < 0) x = 0;
    ssd1306_draw_string(ssd, text, x, y);
}

// Exibe duas linhas de texto centralizadas
void display_two_lines(ssd1306_t *ssd, const char* line1, const char* line2) {
    ssd1306_fill(ssd, false);
    
    int y1 = (DISPLAY_HEIGHT / 2) - 12;
    int y2 = (DISPLAY_HEIGHT / 2) + 4;
    
    write_centered_text(ssd, line1, y1);
    write_centered_text(ssd, line2, y2);
    ssd1306_send_data(ssd);
}

// Quebra texto em múltiplas linhas
void wrap_text(const char* text, char* wrapped_text, int max_width) {
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

// Exibe texto com múltiplas linhas
void display_multiline_text(ssd1306_t *ssd, const char* text) {
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

// Exibe texto com quebra automática
void display_wrapped_message(ssd1306_t *ssd, const char* text) {
    char wrapped_text[256];
    wrap_text(text, wrapped_text, MAX_CHARS_PER_LINE);
    display_multiline_text(ssd, wrapped_text);
}

// Exibe tela inicial
void show_splash_screen(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    write_centered_text(ssd, "ZENITH PROJECT", (DISPLAY_HEIGHT - CHAR_HEIGHT) / 2);
    ssd1306_send_data(ssd);
    sleep_ms(2000);
}
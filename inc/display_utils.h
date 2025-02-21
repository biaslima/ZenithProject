#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include "ssd1306.h"

// Funções para manipulação de texto e display
void write_centered_text(ssd1306_t *ssd, const char* text, int y);
void display_two_lines(ssd1306_t *ssd, const char* line1, const char* line2);
void display_multiline_text(ssd1306_t *ssd, const char* text);
void display_wrapped_message(ssd1306_t *ssd, const char* text);
void to_uppercase(char* str);
void wrap_text(const char* text, char* wrapped_text, int max_width);
void show_splash_screen(ssd1306_t *ssd);

#endif // DISPLAY_UTILS_H
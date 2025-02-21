#ifndef BREATHING_ROUTINES_H
#define BREATHING_ROUTINES_H

#include "ssd1306.h"

typedef enum {
    BREATHING_DIAPHRAGMATIC,
    BREATHING_SQUARE,
    BREATHING_CALM
} breathing_type_t;

typedef struct {
    breathing_type_t type;
    uint32_t inspire_time;
    uint32_t expire_time;
    uint32_t hold_time;
    uint8_t cycles;
} breathing_params_t;

extern breathing_type_t current_breathing_type;

void diaphragmatic_breathing(ssd1306_t *ssd);
void square_breathing(ssd1306_t *ssd);
void calm_breathing(ssd1306_t *ssd);

#endif
#include "config.h"

volatile int current_level = 0;
volatile bool btn_a_ativo = false;
volatile bool btn_b_ativo = false;
volatile uint16_t ambient_noise_level = 0;
volatile uint32_t last_interrupt_time_A = 0;
volatile uint32_t last_interrupt_time_B = 0;

uint32_t leds[NUM_LEDS]; 


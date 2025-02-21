#include "button_handler.h"
#include "hardware/irq.h"
#include "config.h"
#include <stdio.h> 

volatile bool interrupt_routine = false;

void button_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    if (gpio == BTN_PIN_A && current_time - last_interrupt_time_A > 200000) {
        last_interrupt_time_A = current_time;
        btn_a_ativo = true;
    } 
    else if (gpio == BTN_PIN_B && current_time - last_interrupt_time_B > 200000) {
        last_interrupt_time_B = current_time;
        btn_b_ativo = true;
    }
    else if (gpio == JOYSTICK_BTN_PIN) {
        // Quando o botão do joystick é pressionado, ativa a flag de interrupção
        interrupt_routine = true;
        printf("Joystick button pressed - interrupting routine\n");
    }
}

void setup_button_interrupts(void) {
    gpio_set_irq_enabled_with_callback(BTN_PIN_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    
    // Configure os outros botões apenas com enabled (mesma função de callback)
    gpio_set_irq_enabled(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true);
    
    // Configure o botão do joystick
    gpio_init(JOYSTICK_BTN_PIN);
    gpio_set_dir(JOYSTICK_BTN_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BTN_PIN);
    gpio_set_irq_enabled(JOYSTICK_BTN_PIN, GPIO_IRQ_EDGE_FALL, true);
}

#include "button_handler.h"
#include "hardware/irq.h"
#include "config.h"

void button_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (gpio == BTN_PIN_A && current_time - last_interrupt_time_A > 200000) {
        last_interrupt_time_A = current_time;
        btn_a_ativo = true;
    } else if (gpio == BTN_PIN_B && current_time - last_interrupt_time_B > 200000) {
        last_interrupt_time_B = current_time;
        btn_b_ativo = true;
    }
}

void setup_button_interrupts(void) {
    gpio_set_irq_enabled_with_callback(BTN_PIN_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);
}


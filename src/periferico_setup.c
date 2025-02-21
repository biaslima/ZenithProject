#include <stdio.h>
#include <stdlib.h>
#include "pico/time.h"
#include "periferico_setup.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "ws2812.pio.h"
#include "config.h"
#include "hardware/i2c.h" 
#include "pico/stdlib.h" 


void gpio_setup(void) {
    gpio_init(LED_MATRIX_PIN);
    gpio_set_dir(LED_MATRIX_PIN, GPIO_OUT);

    gpio_init(LED_PIN_RED);
    gpio_init(LED_PIN_BLUE);
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    gpio_init(BTN_PIN_A);
    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_A, GPIO_IN);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_A);
    gpio_pull_up(BTN_PIN_B);
}

void pwm_setup(void) {
    gpio_set_function(LED_PIN_RED, GPIO_FUNC_PWM);
    gpio_set_function(LED_PIN_BLUE, GPIO_FUNC_PWM);
    gpio_set_function(LED_PIN_GREEN, GPIO_FUNC_PWM);
    
    uint slice_vermelho = pwm_gpio_to_slice_num(LED_PIN_RED);
    uint slice_azul = pwm_gpio_to_slice_num(LED_PIN_BLUE);
    uint slice_verde = pwm_gpio_to_slice_num(LED_PIN_GREEN);
    
    pwm_set_wrap(slice_vermelho, 255);
    pwm_set_wrap(slice_azul, 255);
    pwm_set_wrap(slice_verde, 255);
    
    pwm_set_enabled(slice_vermelho, true);
    pwm_set_enabled(slice_azul, true);
    pwm_set_enabled(slice_verde, true);
}

void adc_setup(void) {
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2);
}

void display_setup(void) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA); 
    gpio_pull_up(I2C_SCL); 
}

void matrix_setup(void) {
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_MATRIX_PIN, 800000, IS_RGBW);
}

void setup_peripherals(void) {
    gpio_setup();
    pwm_setup();
    adc_setup();
    display_setup();
    matrix_setup();
}

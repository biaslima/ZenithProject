#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/irq.h" 
#include "hardware/pwm.h" 
#include "hardware/adc.h"
#include "ws2812.pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

//Definição de Pinos
#define LED_PIN_RED 13
#define LED_PIN_BLUE 12
#define LED_PIN_GREEN 11
#define BTN_PIN_A 5
#define BTN_PIN_B 6
#define MIC_PIN 28

//Pinos e variáveis display
#define I2C_PORT i2c1 
#define I2C_SDA 14 
#define I2C_SCL 15 
#define endereco 0x3C 

//Pinos e Variáveis da Matriz de LEDs
#define LED_MATRIX_PIN 7
#define NUM_LEDS 25
#define IS_RGBW false
uint32_t leds[NUM_LEDS];

// Variáveis globais
volatile int current_level = 0; // 0: Nenhum, 1: Leve, 2: Médio, 3: Grave
volatile bool btn_a_ativo = false;
volatile bool btn_b_ativo = false;
volatile uint16_t ambient_noise_level = 0;
static volatile uint32_t last_interrupt_time_A = 0;  
static volatile uint32_t last_interrupt_time_B = 0;  

//=====================================Inicialição dos periféricos ==============================
//GPIOS
void gpio_setup(){
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
//PWM
void pwm_setup() {
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
//ADC - Microfone
void adc_setup(){
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(0);
}
//Display OLED
void display_setup(){
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA); 
    gpio_pull_up(I2C_SCL); 
    ssd1306_t ssd; 
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd); 
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}
//Matriz de LEDs
void matrix_setup(){
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_MATRIX_PIN, 800000, IS_RGBW);
}
//=====================================Funções da Matriz de LED==================================
uint8_t localizar_led_xy(uint8_t x, uint8_t y) {
    return y * 5 + x;
}
// Função para criar uma cor GRB
uint32_t create_color(uint8_t green, uint8_t red, uint8_t blue) {
    return ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
}
//=======================Outras Funções===============================
//Callback da Interrupção
void button_callback(uint gpio, uint32_t events){

    uint32_t current_time = to_us_since_boot(get_absolute_time());


    if (gpio == BTN_PIN_A && current_time - last_interrupt_time_A > 200000) {
        last_interrupt_time_A = current_time;
        btn_a_ativo = true;
    } else if (gpio == BTN_PIN_B && current_time - last_interrupt_time_B > 200000) {
        last_interrupt_time_B = current_time;
        btn_b_ativo = true;
    }
}
//Definir nível de ansiedade
void determine_anxiety_level() {
    if (btn_a_ativo && btn_b_ativo) {
        current_level = 3; // Nível Grave
    } else if (btn_a_ativo && !btn_b_ativo) {
        current_level = 1; // Nível Leve
    } else if (btn_b_ativo && !btn_a_ativo) {
        current_level = 2; // Nível Médio
    } else {
        current_level = 0; // Nenhum nível selecionado
    }
    btn_a_ativo = false;
    btn_b_ativo = false;
}
// Escrever no display 
void write_to_display(ssd1306_t *ssd, const char* text) {
    ssd1306_fill(ssd, false);  // Limpa o display
    ssd1306_draw_string(ssd, text, 50, 30);
    ssd1306_send_data(ssd);
}
void test_button_reading() {
    printf("Teste de leitura dos botoes:\n");
    printf("Pressione os botoes A, B, ou ambos para testar.\n");
    printf("O teste durara 10 segundos.\n");

    for (int i = 0; i < 10; i++) {
        sleep_ms(1000);
        determine_anxiety_level();
        printf("Nivel de ansiedade: %d\n", current_level);
    }

    printf("Teste de botoes concluido.\n");
}

int main(){

    stdio_init_all();
    gpio_setup();
    pwm_setup();
    adc_setup();
    display_setup();

    // Inicialização do display
    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);

    //Configurações das interrupções 
    gpio_set_irq_enabled_with_callback(BTN_PIN_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // Teste de leitura dos botões
    test_button_reading();

    while (true) {
        determine_anxiety_level();

        char display_text[20];
        snprintf(display_text, sizeof(display_text), "NIVEL: %d", current_level);
        write_to_display(&ssd, display_text);

        printf("Nivel de ansiedade: %d\n", current_level);
        sleep_ms(1000);

        switch (current_level) {
            case 1:
                // Lógica para nível leve
                break;
            case 2:
                // Lógica para nível médio
                break;
            case 3:
                // Lógica para nível grave
                break;
            default:
                // Nenhum nível selecionado
                break;
        }
    }
}
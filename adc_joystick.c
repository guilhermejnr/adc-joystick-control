#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDR 0x3C 

#define JOYSTICK_X_PIN 27 
#define JOYSTICK_Y_PIN 26 
#define JOYSTICK_PB 22

#define BUTTON_A 5        
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12

#define PWM_FREQ 50
#define PWM_WRAP 4095

// Definir uma zona morta mais ampla para o joystick
#define JOYSTICK_DEADZONE 200

// Variáveis globais
ssd1306_t ssd;
volatile bool led_enabled = true;
volatile bool led_green_state = false;
volatile bool border_style = true;  // true = sólido, false = pontilhado
volatile int border_size = 2;

volatile uint32_t ultimo_tempo_joy = 0;
volatile uint32_t ultimo_tempo_A = 0;
const uint32_t debounce = 200; 

// Função para mapear valores de um intervalo para outro
uint16_t map(uint16_t value, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Função para desenhar a borda com o estilo atual
void draw_border(ssd1306_t *ssd) {
    if (border_style) {
        // Borda sólida
        for (int i = 0; i < border_size; i++) {
            ssd1306_rect(ssd, i, i, WIDTH - (2 * i), HEIGHT - (2 * i), true, false);
        }
    } else {
        // Borda pontilhada
        for (int i = 0; i < HEIGHT; i += 2) {
            ssd1306_pixel(ssd, 0, i, true);
            ssd1306_pixel(ssd, WIDTH - 1, i, true);
        }
        for (int i = 0; i < WIDTH; i += 2) {
            ssd1306_pixel(ssd, i, 0, true);
            ssd1306_pixel(ssd, i, HEIGHT - 1, true);
        }
    }
}

// Função de interrupção para detectar pressionamento dos botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (gpio == JOYSTICK_PB && (tempo_atual - ultimo_tempo_joy > debounce)) {
        ultimo_tempo_joy = tempo_atual;
        
        // Alterna estado do LED verde
        led_green_state = !led_green_state; 
        gpio_put(LED_GREEN, led_green_state);
        
        // Alterna entre estilos de borda
        if (border_size == 2 && border_style == true) {
            border_size = 2;
            border_style = false;  // Muda para pontilhado
        } else if (border_size == 2 && border_style == false) {
            border_size = 4;
            border_style = true;   // Muda para sólido grosso
        } else {
            border_size = 2;
            border_style = true;   // Volta para sólido fino
        }
    } 
    else if (gpio == BUTTON_A && (tempo_atual - ultimo_tempo_A > debounce)) {
        ultimo_tempo_A = tempo_atual;
        led_enabled = !led_enabled;
    }
}

void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin); 
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_clkdiv(slice, 125.0); 
    pwm_set_enabled(slice, true);
}

void set_led_brightness(uint pin, uint16_t value) {
    uint slice = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);
    pwm_set_chan_level(slice, channel, value);
}

// Função para atualizar o display com a posição do joystick e a borda
void update_display(ssd1306_t *ssd, uint8_t pos_x, uint8_t pos_y) {
    ssd1306_fill(ssd, false);
    
    // Desenha o quadrado que representa a posição do joystick
    ssd1306_rect(ssd, pos_y, pos_x, 8, 8, true, true);
    
    // Desenha a borda de acordo com o estilo atual
    draw_border(ssd);
    
    ssd1306_send_data(ssd);
}

// Função para controlar os LEDs com base nos valores do joystick
void update_leds(uint16_t adc_x, uint16_t adc_y) {
    uint16_t pwm_x = 0;
    uint16_t pwm_y = 0;
    
    if (abs(adc_x - 2048) > JOYSTICK_DEADZONE) {
        // Mapeia o desvio do centro para o valor de PWM
        pwm_x = led_enabled ? map(abs(adc_x - 2048), JOYSTICK_DEADZONE, 2048, 0, PWM_WRAP) : 0;
        set_led_brightness(LED_RED, pwm_x);
    } else {
        set_led_brightness(LED_RED, 0);
    }
    
    if (abs(adc_y - 2048) > JOYSTICK_DEADZONE) {
        // Mapeia o desvio do centro para o valor de PWM
        pwm_y = led_enabled ? map(abs(adc_y - 2048), JOYSTICK_DEADZONE, 2048, 0, PWM_WRAP) : 0;
        set_led_brightness(LED_BLUE, pwm_y);
    } else {
        set_led_brightness(LED_BLUE, 0);
    }
}

int main() {
    stdio_init_all();
    
    // Inicializa I2C para o display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa o display SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    // Inicializa o ADC para ler o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    // Configura o botão do joystick com interrupção
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    // Configura o botão A com interrupção
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    // Configura os LEDs (RED e BLUE com PWM, GREEN como GPIO)
    setup_pwm(LED_RED);
    setup_pwm(LED_BLUE);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    
    uint16_t adc_x, adc_y;
    while (true) {
        // Lê os valores do joystick
        adc_select_input(1);  // Canal para o eixo X
        adc_x = adc_read();
        
        adc_select_input(0);  // Canal para o eixo Y
        adc_y = adc_read();
        
        // Atualiza os LEDs com base nos valores do joystick
        update_leds(adc_x, adc_y);
        
        // Calcula a posição do quadrado no display
        uint8_t pos_x = (adc_x * (WIDTH - 8)) / 4095;
        uint8_t pos_y = ((4095 - adc_y) * (HEIGHT - 8)) / 4095;
        
        // Atualiza o display
        update_display(&ssd, pos_x, pos_y);
        
        // Pequena pausa para não sobrecarregar o processador
        sleep_ms(50);
    }
}
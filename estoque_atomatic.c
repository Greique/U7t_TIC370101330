 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/irq.h"
 #include "hardware/i2c.h"
 #include "hardware/pwm.h"
 #include "hardware/clocks.h"
 #include "inc/ssd1306.h"
 #include "inc/font.h"
 #include "matriz_led.h"
 
 #define LED_VERDE_PIN 11
 #define LED_AZUL_PIN 12
 #define LED_SETOR_303_PIN 13
 #define BOTAO_A_PIN 5 
 #define I2C_PORT i2c1
 #define I2C_SDA 14
 #define I2C_SCL 15
 #define endereco 0x3C
 #define SERVO_PIN 22  
 #define FREQ_PWM 50   
 #define PERIOD 20000
 #define BUZZER_PIN 21
 #define OUT_PIN 7 // Definição explícita do pino da matriz de LEDs WS2812
 #define TEMPO_POR_METRO 1000 // Tempo em ms para percorrer 1 metro

 #define BUZZER_FREQUENCY 1000
 
 typedef struct {
     bool led_verde_estado;
     bool em_movimento;
     ssd1306_t ssd;
     PIO pio;
     uint sm;
 } sistema_t;
 PIO pio = pio0;
 uint sm;
 sistema_t sistema = {false, false, {0}, pio0, 0};

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Configuração da matriz de LEDs WS2812
void setup_ws2812() {
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(50); // Pausa de 100ms
}

 
 void inicializar_display() {
     i2c_init(I2C_PORT, 400 * 1000);
     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(I2C_SDA);
     gpio_pull_up(I2C_SCL);
     ssd1306_init(&sistema.ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
     ssd1306_config(&sistema.ssd);
     ssd1306_fill(&sistema.ssd, false);
     ssd1306_send_data(&sistema.ssd);
 }
 
 void setup_pwm() {
     gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
     uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
     pwm_config config = pwm_get_default_config();
     pwm_config_set_clkdiv(&config, 64.0f);
     pwm_config_set_wrap(&config, clock_get_hz(clk_sys) / (FREQ_PWM * 64));
     pwm_init(slice_num, &config, true);
 }
 
 void set_pwm_duty(float duty_us) {
     uint16_t wrap_value = clock_get_hz(clk_sys) / (FREQ_PWM * 64);
     uint16_t level = (duty_us / PERIOD) * wrap_value;
     pwm_set_gpio_level(SERVO_PIN, level);
 }

 void acionar_setor(int setor) {
     sistema.em_movimento = true;
 
     // Apagar o LED da matriz antes de iniciar o novo setor
     exibir_numero(0, pio, sm, 0.0, 0.0, 0.0);
 
     int distancia_metros = 0;
 
     if (setor == 101) {
         distancia_metros = 5; // Setor 101 percorre 5 metros
         for (int i = 0; i < 5; i++) {
             gpio_put(LED_AZUL_PIN, 1);
             sleep_ms(200);
             gpio_put(LED_AZUL_PIN, 0);
             sleep_ms(200);
         }
         set_pwm_duty(2400);
         sleep_ms(distancia_metros * TEMPO_POR_METRO);
         set_pwm_duty(500);
         gpio_put(LED_AZUL_PIN, 1);
 
     } else if (setor == 303) {
         distancia_metros = 10; // Setor 303 percorre 10 metros
         for (int i = 0; i < 5; i++) {
             gpio_put(LED_SETOR_303_PIN, 1);
             sleep_ms(200);
             gpio_put(LED_SETOR_303_PIN, 0);
             sleep_ms(200);
         }
         set_pwm_duty(2400);
         sleep_ms(distancia_metros * TEMPO_POR_METRO);
         set_pwm_duty(500);
         gpio_put(LED_SETOR_303_PIN, 1);
         
     } else {
         printf("Código inválido! Aperte o botão A para inserir outro código.\n");
         for (int i = 0; i < 5; i++) {
             gpio_put(LED_VERDE_PIN, 1);
             sleep_ms(100);
             gpio_put(LED_VERDE_PIN, 0);
             sleep_ms(100);
             beep(BUZZER_PIN, 500);
         }
         return;
     }
 
     // Aguarda 5 segundos e apaga o LED da matriz
     sleep_ms(5000);
     exibir_numero(0, pio, sm, 0.0, 0.0, 0.0);
     ssd1306_fill(&sistema.ssd, false);
     ssd1306_draw_string(&sistema.ssd, "PRODUTO ACEITO", 10, 20);
     ssd1306_send_data(&sistema.ssd);
     
 }
 
 
 void botao_callback(uint gpio, uint32_t eventos) {
     static uint32_t ultimo_tempo = 0;
     uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
     if (tempo_atual - ultimo_tempo < 200) return;

     ultimo_tempo = tempo_atual;
     if (gpio == BOTAO_A_PIN) {
         sistema.em_movimento = false;
         gpio_put(LED_AZUL_PIN, 0);
         gpio_put(LED_SETOR_303_PIN, 0);
         exibir_numero(1, pio, sm, 0.5, 0.0, 0.0);
         char mensagem[20];
     }
 }
 
 void configurar_interrupcoes() {
     gpio_set_irq_enabled_with_callback(BOTAO_A_PIN, GPIO_IRQ_EDGE_FALL, true, &botao_callback);
 }
 
 int main() {
    stdio_init_all();
    inicializar_display();
    setup_pwm();
    setup_ws2812();

    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_init(LED_AZUL_PIN);
    gpio_set_dir(LED_AZUL_PIN, GPIO_OUT);
    gpio_init(LED_SETOR_303_PIN);
    gpio_set_dir(LED_SETOR_303_PIN, GPIO_OUT);
    gpio_init(BOTAO_A_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);
    pwm_init_buzzer(BUZZER_PIN);

    configurar_interrupcoes();

    char entrada[13] = {0};
    int contador = 0;
    absolute_time_t tempo_inicio = get_absolute_time();  // Armazena o tempo inicial

    while (true) {
        if (!sistema.em_movimento) {
            int c = getchar_timeout_us(0);

            // Verifica se o codigo é valido.
            if (contador > 0 && absolute_time_diff_us(tempo_inicio, get_absolute_time()) > 500000) {
                printf("INVÁLIDO! Insira o código novamente.\n");
                ssd1306_fill(&sistema.ssd, false);
                ssd1306_draw_string(&sistema.ssd, "INVALIDO", 10, 20);
                ssd1306_send_data(&sistema.ssd);
                beep(BUZZER_PIN, 300);
                exibir_numero(1, pio, sm, 0.0, 0.5, 0.0);
                contador = 0;
                memset(entrada, 0, sizeof(entrada));  // Limpa o buffer
                continue;
            }

            if (c != PICO_ERROR_TIMEOUT && c >= '0' && c <= '9') {
                if (contador == 0) {
                    tempo_inicio = get_absolute_time();  // Reseta o tempo no primeiro caractere
                }
                
                if (contador < 13) {
                    entrada[contador++] = c;
                    entrada[contador] = '\0';
                }

                if (contador == 13) {
                    ssd1306_fill(&sistema.ssd, false);
                    ssd1306_draw_string(&sistema.ssd, entrada, 10, 20);
                    ssd1306_send_data(&sistema.ssd);
                    printf("Codigo de barra: %s\n", entrada);

                    char setor_str[4] = {entrada[0], entrada[1], entrada[2], '\0'};
                    int setor = atoi(setor_str);

                    if (!sistema.em_movimento) {
                        acionar_setor(setor);
                    }
                    
                    contador = 0;
                    memset(entrada, 0, sizeof(entrada));  // Limpa o buffer após processamento
                }
            }
        }
        sleep_ms(10);
    }
}

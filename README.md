# Funcionalidades do Sistema de Automacao

Este sistema de automação permite o controle de movimentação para setores específicos dentro de um estoque automatizado, utilizando um Raspberry Pi Pico W e diversos componentes embarcados.

## Funcionalidades Principais

### 1. Leitura de Código de Barras
- O sistema recebe um código de barras via entrada serial.
- O código é validado antes do processamento.
- Se o código for inválido, um alerta é emitido no display OLED, matriz de LED e um sinal sonoro é ativado.

### 2. Acionamento de Setores
- Após a leitura do código de barras, os primeiros três dígitos determinam o setor.(EX: 101 e 303)
- Se o setor for reconhecido, o sistema inicia o deslocamento do carrinho até o local correspondente.
- O LED correspondente ao setor pisca durante o deslocamento.
- Após a chegada ao setor, o LED correspondente fica aceso.

### 3. Controle de Movimento via PWM
- O deslocamento do carrinho é controlado por um servo motor utilizando PWM.
- O tempo de deslocamento é proporcional à distância do setor.
- O servo motor altera sua posição ao chegar no setor e ao retornar.

### 4. Indicação Visual e Sonora
- LEDs indicam o estado do sistema:
  - LED azul pisca para indicar movimentação para o setor 101.
  - LED do setor 303 pisca para indicar movimentação para esse setor.
  - LED verde pisca em caso de código inválido.
- Um buzzer emite um som para alertar sobre códigos inválidos.

### 5. Exibição de Informações no Display OLED
- O display OLED exibe mensagens relevantes:
  - Código de barras inserido.
  - Mensagem de erro para código inválido.
  - Mensagem "PRODUTO ACEITO" após o transporte bem-sucedido.

### 6. Controle de Matriz de LEDs WS2812
- A matriz de LEDs exibe informações sobre o estado do sistema.
- Antes da movimentação, os LEDs são apagados.
- Durante a movimentação, a matriz pode exibir números ou padrões indicativos.

### 7. Interrupção por Botão
- O botão A funciona como o escaner de codigo de barra, quando ativo sinal que o codigo corresponde ao do produto.
- Quando pressionado, os LEDs de setor são apagados e o sistema retorna ao estado inicial ou para o proximo setor.

## Componentes Utilizados
- **Raspberry Pi Pico W**
- **Display OLED SSD1306** via I2C
- **Matriz de LEDs WS2812**
- **LEDs Indicadores** (Verde, Azul, Vermelho)
- **Servo Motor** para movimentação
- **Buzzer** para alertas sonoros
- **Botão de Interrupção** para simular o leitor de codigo

Este sistema permite a automação eficiente do transporte de peças dentro do estoque, reduzindo erros e custos aumentando a produtividade.

## Video do projeto ##

https://drive.google.com/file/d/1sXVEDODd1OoDo4BQjpkO-4d4xez635lz/view?usp=sharing

# Projeto BitDogLab: Controle de LEDs via Joystick e Visualização no Display OLED

## Descrição do Projeto

Este projeto demonstra a integração do joystick analógico com LEDs RGB e display OLED utilizando a plataforma BitDogLab baseada no microcontrolador RP2040 (Raspberry Pi Pico). O sistema captura valores analógicos do joystick para controlar a intensidade dos LEDs RGB via PWM e exibir a posição atual em um display OLED SSD1306.

## Funcionalidades Implementadas

1. **Controle de LEDs via Joystick:**
   - LED Vermelho: Controlado pelo movimento horizontal do joystick (eixo X)
   - LED Azul: Controlado pelo movimento vertical do joystick (eixo Y)
   - Quanto maior o deslocamento do joystick a partir do centro, maior a intensidade do LED

2. **Resposta a Botões (via Interrupções):**
   - Botão do Joystick: Alterna o estado do LED Verde e modifica o estilo da borda no display
   - Botão A: Ativa/desativa o controle PWM dos LEDs

3. **Visualização no Display OLED:**
   - Exibe um quadrado de 8x8 pixels que se move de acordo com a posição do joystick
   - Implementa diferentes estilos de borda que mudam conforme o botão do joystick é pressionado

## Demonstração

[https://drive.google.com/file/d/1pwYnqhts_dnXXSEYvHHiwihxnJDA0TSw/view?usp=sharing]

## Componentes Utilizados

- Placa BitDogLab com RP2040
- Display OLED SSD1306 (128x64)
- LED RGB
- Joystick analógico de 2 eixos com botão
- Botão adicional (Botão A)

## Pinagem

| Componente      | GPIO          | Função                            |
|-----------------|---------------|-----------------------------------|
| LED RGB         | 11, 12, 13    | Controle dos LEDs Verde, Azul, Vermelho |
| Botão Joystick  | 22            | Entrada com pull-up              |
| Joystick        | 26, 27        | ADC para eixos Y e X              |
| Botão A         | 5             | Entrada com pull-up              |
| Display SSD1306 | 14, 15        | I2C (SDA, SCL)                   |

## Conceitos Aplicados

1. **Leitura Analógica (ADC)**:
   - Conversão dos valores do joystick (0-4095) para posições no display
   - Mapeamento dos valores analógicos para PWM

2. **Modulação por Largura de Pulso (PWM)**:
   - Controle da intensidade dos LEDs RGB

3. **Interface I2C**:
   - Comunicação com o display OLED SSD1306

4. **Interrupções**:
   - Tratamento de eventos de botões com debounce em software
   - Alternância de estados dos LEDs e estilos de borda

## Estrutura do Projeto

```
adc_joystick/
│
├── biblioteca/
│   ├── font.h              # Definição de fontes para o display
│   ├── ssd1306.c           # Implementação das funções do display
│   └── ssd1306.h           # Definições e protótipos para o display
│
├── adc_joystick.c          # Arquivo principal com a lógica do programa
├── CMakeLists.txt          # Configuração de compilação
├── diagram.json            # Diagrama de blocos representando o sistema
├── pico_sdk_import.cmake   # Arquivo de importação do SDK do Raspberry Pi Pico
└── wokwi.toml              # Arquivo de configuração para simulação no Wokwi

```

## Como Compilar e Executar

1. **Preparação do Ambiente:**
   
    Defina o caminho para o Raspberry Pi Pico SDK
    ```bash
    export PICO_SDK_PATH=/caminho/para/pico-sdk
    ```

Crie o diretório de build e entre nele
```bash
mkdir build
cd build
```

Configure o projeto usando o Ninja como gerador
```bash
cmake -G "Ninja" ..
 ```

2. **Compilação:**
```bash
  Compile o projeto usando o Ninja
  ninja
```

3. **Gravação:**
   ```bash
    Conecte a placa BitDogLab no modo de programação (BOOTSEL pressionado durante reset)
    cp adc_joystick.uf2 /caminho/para/RPI-RP2
   ```

**Nota:** Este projeto foi desenvolvido com o objetivo de aplicar os conceitos de programação em sistemas embarcados utilizando o RP2040, com foco na integração de periféricos analógicos e digitais.
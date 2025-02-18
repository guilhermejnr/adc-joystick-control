# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(adc_joystick C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(adc_joystick 
    adc_joystick.c
    biblioteca/ssd1306.c  # Caminho correto para ssd1306.c
)

pico_set_program_name(adc_joystick "adc_joystick")
pico_set_program_version(adc_joystick "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(adc_joystick 1)
pico_enable_stdio_usb(adc_joystick 1)

# Add the standard library to the build
target_link_libraries(adc_joystick
    pico_stdlib
    hardware_adc    # Para o joystick analógico
    hardware_pwm    # Para controle dos LEDs RGB
    hardware_i2c    # Para o display SSD1306
)

# Adicionando a pasta biblioteca/ ao caminho de inclusão
target_include_directories(adc_joystick PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/biblioteca
)

pico_add_extra_outputs(adc_joystick)

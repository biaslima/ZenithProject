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
set(PICO_BOARD pico CACHE STRING "Board type")

# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
include(pico_sdk_import.cmake)

project(ZenithProject C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adicionar executável. Note os caminhos dos arquivos fonte
add_executable(ZenithProject 
    src/ZenithProject.c
    src/periferico_setup.c
    src/button_handler.c
    src/anxiety_level.c
    src/ssd1306.c
    src/global_vars.c
    src/breathing_routines.c
    src/led_animation.c  
    src/noise_detection.c
)

# Incluir diretórios de cabeçalhos
target_include_directories(ZenithProject PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# Configurar nome e versão do programa
pico_set_program_name(ZenithProject "ZenithProject")
pico_set_program_version(ZenithProject "0.1")

# Gerar cabeçalho PIO para a matriz de LEDs
pico_generate_pio_header(ZenithProject ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio)

# Habilitar saída via UART e USB para depuração
pico_enable_stdio_uart(ZenithProject 1)
pico_enable_stdio_usb(ZenithProject 1)

# Adicionar bibliotecas padrão ao projeto
target_link_libraries(ZenithProject 
    pico_stdlib
    hardware_pio
    hardware_gpio
    hardware_clocks
    hardware_i2c
    hardware_irq
    hardware_pwm
    hardware_adc
)

# Gerar saídas extras (como .uf2)
pico_add_extra_outputs(ZenithProject)



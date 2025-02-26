# Projeto Zenith - Sistema Embarcado para Controle de Ansiedade

## 📋 Visão Geral
O **Projeto Zenith** é um sistema embarcado desenvolvido para auxiliar pessoas com problemas de ansiedade através de técnicas guiadas de respiração. Utilizando feedback visual por meio de LEDs e instruções em um display OLED, o dispositivo ajuda o usuário a controlar sua respiração seguindo padrões cientificamente eficazes na redução de sintomas de ansiedade.

## 🎥 Demonstração
📽️ Vídeo de demonstração do projeto: [Insira o link aqui]

## ✨ Funcionalidades
O dispositivo permite ao usuário selecionar uma das três rotinas de respiração:

- **Respiração Diafragmática**: Inspiração por 4 segundos e expiração por 6 segundos, enfatizando o uso do diafragma.
- **Respiração Quadrada**: Inspiração por 4 segundos, retenção por 4 segundos, expiração por 4 segundos e retenção com pulmões vazios por 4 segundos.
- **Respiração 4-7-8 (Calm Breathing)**: Inspiração por 4 segundos, retenção por 7 segundos e expiração lenta por 8 segundos.

### Recursos adicionais:
✅ Feedback visual sincronizado com LEDs e matriz de LEDs  
✅ Detecção de ambiente para verificar níveis de ruído inadequados  
✅ Interface simples com apenas dois botões e um joystick  
✅ Tela OLED para instruções detalhadas  

## 🛠️ Hardware Utilizado
- **Microcontrolador RP2040** (BitDogLab)
- **Display OLED SSD1306** (128x64 pixels)
- **LED RGB** para feedback visual
- **Matriz de LEDs WS2812** (5x5)
- **Botões** para seleção de modo
- **Joystick** para interrupção de rotinas
- **Microfone** para detecção de ambiente

## 📁 Estrutura do Repositório
```
├── inc/                     # Arquivos de cabeçalho (.h)
│   ├── anxiety_level.h
│   ├── breathing_routines.h
│   ├── button_handler.h
│   ├── config.h              # Definições e configurações globais
│   ├── display_utils.h
│   ├── font.h                # Fonte para o display OLED
│   ├── led_animation.h
│   ├── noise_detection.h
│   ├── periferico_setup.h
│   └── ssd1306.h             # Driver do display OLED
│
├── src/                     # Arquivos de implementação (.c)
│   ├── anxiety_level.c
│   ├── breathing_routines.c
│   ├── button_handler.c
│   ├── display_utils.c
│   ├── global_vars.c         # Variáveis globais
│   ├── led_animation.c
│   ├── noise_detection.c
│   ├── periferico_setup.c
│   ├── ssd1306.c
│   ├── ws2812.pio            # Programa PIO para controle da matriz de LEDs
│   └── ZenithProject.c       # Arquivo principal com função main
│
├── build/                   # Diretório gerado para os arquivos de build
├── .vscode/                 # Configurações do VS Code
├── CMakeLists.txt           # Configuração do CMake para compilação
├── pico_sdk_import.cmake    # Importação do SDK do RP2040
├── diagram.json             # Diagrama do projeto
├── Documentacao.pdf         # Documentação completa do projeto
├── .gitignore               # Arquivos ignorados pelo git
└── README.md                # Este arquivo
```

## 📥 Pré-requisitos
Para compilar e executar este projeto, você precisará:
- SDK do Raspberry Pi Pico (RP2040)
- CMake 3.13 ou superior
- Compilador ARM GCC
- Placa BitDogLab com RP2040
- Componentes de hardware conforme listados acima

## 🔧 Instalação e Compilação

Clone o repositório:
```bash
git clone https://github.com/biaslima/ZenithProject.git
cd ZenithProject
```

Crie o diretório de build e compile o projeto:
```bash
mkdir build
cd build
cmake ..
make
```

Conecte a placa **BitDogLab** ao computador enquanto pressiona o botão **BOOTSEL**.  
Copie o arquivo **uf2** gerado para a unidade **RPI-RP2** que aparecerá.

## 📝 Instruções de Uso

### Configuração Inicial
1. Conecte todos os componentes conforme o diagrama na documentação.
2. Alimente o dispositivo via USB ou bateria.
3. Ao ligar, o Zenith exibirá a tela de boas-vindas.

### Seleção de Rotina
- **Botão A**: Respiração Diafragmática
- **Botão B**: Respiração Quadrada
- **Ambos os botões simultaneamente**: Respiração 4-7-8

### Durante o Exercício
✅ Siga as instruções no display OLED  
✅ Observe o padrão dos LEDs para guiar sua respiração  
✅ O dispositivo detectará ambientes ruidosos e alertará caso necessário  

### Interrompendo uma Sessão
- Pressione o **botão do joystick** a qualquer momento para cancelar a sessão.

## 📊 Melhores Práticas
✔️ Utilize o dispositivo em um ambiente tranquilo.  
✔️ Sente-se confortavelmente ou deite-se.  

## 🤝 Contribuição
Contribuições são bem-vindas! Siga os passos abaixo:
1. Faça um fork do projeto.
2. Crie sua branch de feature: `git checkout -b feature/NovaFuncionalidade`
3. Commit suas mudanças: `git commit -m 'Adiciona nova funcionalidade'`
4. Push para a branch: `git push origin feature/NovaFuncionalidade`
5. Abra um **Pull Request**.

🚀 Desenvolvido como parte do programa **EmbarcaTech**


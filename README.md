# firmware_lib-my-fs

Essa biblioteca foi construída para gerenciar arquivos da memória flash no esp32, seus métodos foram organizados em formato de classe com o nome MyFS e o mebro estático myFS.

## Estrutura de arquivos

firmware_lib-my-fs/
│   .gitignore
│   library.json
│   README.txt
├───.pio
│   └───build
│       │   project.checksum
│       └───wt32-sc01-plus
│               idedata.json
├───.vscode
│       c_cpp_properties.json
│       extensions.json
│       launch.json
├───examples
│       ex_01.cpp
│       ex_02.cpp
└───src
        MyFS.cpp
        MyFS.h

## Instruções de uso

A biblioteca é baseada no LittleFS e precisa que esse sistema de arquivos seja definido no platformio.ini com o comando abaixo:

```ini
board_build.filesystem = littlefs
```

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

## Ferramente de Download de FS da ESP32

O FileSystem dentro da Flash do ESP32 pode ser lido dentro do Platformio por meio de um script python do projeto [pio-esp32-esp8266-filesystem-downloader](https://github.com/maxgerhardt/pio-esp32-esp8266-filesystem-downloader). </br>
Para usar o script:
1. Baixe o script `download_fs.py` dentro do [repositório acima](https://github.com/maxgerhardt/pio-esp32-esp8266-filesystem-downloader)
2. Mova para o diretório raiz do seu projeto no Platformio
3. Inclua no script `platformio.ini` o parâmetro:
   ```ini
   extra_scripts = download_fs.py
   ```
5. Reinicie o VSCode
6. Dentro da barra de Ferramentas do Platformio, na aba de Tarefas de Projeto, haverá um seção `Custom`, com a opção `Download Filesystem`: </br> ![Botão do Script](https://github.com/maxgerhardt/pio-esp32-esp8266-filesystem-downloader/blob/main/project_task.png)
7. Clique em `Download Filesystem` para baixar arquivos da Flash, prezervando a estrutura de arquivos e diretórios presente, dentro da pasta `unpacked_fs` (diretório pode ser configurado, olhar instruções no projeto).

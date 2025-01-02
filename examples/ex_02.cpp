#include <Arduino.h>
#include <MyFS.h>
#include <SPI.h>

SPIClass spi = SPIClass();

void setup() {
    Serial.begin(115200);
    delay(1000);

    spi.begin(39, 38, 40, 41); // Pinos: SCK, MISO, MOSI, CS

    // Inicializa o SD Card
    if (myFS.SDinit(41, spi)) {
        Serial.println("SD Card inicializado com sucesso!");
    } else {
        Serial.println("Falha ao inicializar o SD Card!");
        ESP.restart(); // reinicia a esp32 [opcional]
    }

    // Lista os arquivos no diretório raiz
    myFS.printDir("/", 2); // Exibe até 2 níveis de profundidade
}

void loop() {
    // Não faz nada
}

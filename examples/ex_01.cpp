#include <Arduino.h>
#include <MyFS.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Inicializa o LittleFS
    if (myFS.LittleFSinit()) {
        Serial.println("LittleFS inicializado com sucesso!");
    } else {
        Serial.println("Falha ao inicializar o LittleFS!");
        ESP.restart(); // reinicia a esp32 [opcional]
    }

    // Escreve dados em um arquivo
    const char *filePath = "/example.txt";
    const char *content = "Este é um exemplo de gravação no LittleFS.";

    if (myFS.writeFile(filePath, content, true)) {
        Serial.println("Arquivo gravado com sucesso!");
    } else {
        Serial.println("Erro ao gravar o arquivo!");
    }

    // Lê os dados do arquivo
    String fileContent = myFS.readFile(filePath, true).c_str();
    Serial.println("Conteúdo do arquivo:");
    Serial.println(fileContent);
}

void loop() {
    // Não faz nada
}

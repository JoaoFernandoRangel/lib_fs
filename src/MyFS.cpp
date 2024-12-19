#include "MyFS.h"

/// @brief Inicializa LittleFS
/// @return retorna se foi inicializado corretamente
bool MyFS::LittleFSinit() {
    if (LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        fs = LittleFS;
        return true;
    } else {
        return false;
    }
}

/// @brief Inicializa SD card
/// @param cs Número do Chip select
/// @param com configuração SPI
/// @return retorna se o SD está montado no sistema
bool MyFS::SDinit(uint8_t cs, SPIClass &com) {
    if (SD.begin(cs, com)) {
        fs = SD;
        return true;
    } else {
        return false;
    }
}

string MyFS::readFile(const char *path, bool debug) {
    string fileContent = "";

    fs::File file = fs.open(path);
    if (!file || file.isDirectory()) {
        SERIAL_OUT.println("\n- failed to open file for reading\n");
        return fileContent;
    }
    while (file.available()) {
        char c = file.read();
        fileContent += c;
    }

    file.close();

    if (debug) {
        SERIAL_OUT.println(fileContent.c_str());
    }
    return fileContent;
}

/// @brief imprime na tela os diretórios de uma pasta
/// @param dirname caminho do diretório
/// @param levels quantidade de níveis que serão mostrados
void MyFS::printDir(const char *dirname, uint8_t levels) {
    SERIAL_OUT.printf("Listing directory: %s\r\n", dirname);

    fs::File root = fs.open(dirname);
    if (!root) {
        SERIAL_OUT.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        SERIAL_OUT.println(" - not a directory");
        return;
    }

    fs::File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            SERIAL_OUT.print("  DIR : ");

            SERIAL_OUT.print(file.name());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            SERIAL_OUT.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900,
                              (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                              tmstruct->tm_sec);

            if (levels) {
                printDir(file.name(), levels - 1);
            }
        } else {
            SERIAL_OUT.print("  FILE: ");
            SERIAL_OUT.print(file.name());
            SERIAL_OUT.print("  SIZE: ");

            SERIAL_OUT.print(file.size());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            SERIAL_OUT.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900,
                              (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                              tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

/// @brief salva em jSON os diretórios de uma pasta
/// @param dirname caminho do diretório
/// @param levels quantidade de níveis que serão mostrados
cJSON *MyFS::listDir(const char *dirname, uint8_t levels) {
    // Cria o objeto JSON raiz
    cJSON *rootJson = cJSON_CreateObject();
    cJSON *filesArray = cJSON_CreateArray();
    cJSON_AddStringToObject(rootJson, "directory", dirname);
    cJSON_AddItemToObject(rootJson, "contents", filesArray);

    fs::File root = fs.open(dirname);
    if (!root) {
        SERIAL_OUT.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        SERIAL_OUT.println(" - not a directory");
        return;
    }

    fs::File file = root.openNextFile();
    while (file) {
        cJSON *fileEntry = cJSON_CreateObject();
        cJSON_AddStringToObject(fileEntry, "name", file.name());
        cJSON_AddBoolToObject(fileEntry, "isDirectory", file.isDirectory());

        if (!file.isDirectory()) {
            cJSON_AddNumberToObject(fileEntry, "size", file.size());
        }

        // Obtém a data/hora da última modificação
        time_t t = file.getLastWrite();
        struct tm *tmstruct = localtime(&t);
        if (tmstruct) {
            char lastWrite[20];
            snprintf(lastWrite, sizeof(lastWrite), "%04d-%02d-%02d %02d:%02d:%02d", tmstruct->tm_year + 1900,
                     tmstruct->tm_mon + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
            cJSON_AddStringToObject(fileEntry, "lastWrite", lastWrite);
        }

        cJSON_AddItemToArray(filesArray, fileEntry);

        file = root.openNextFile();
    }

    return rootJson;
}

/// @brief deletes files of path folder
/// @param path caminho do arquivo ou diretório [deleta todos os arquivos]
void MyFS::removeFile(const char *path) {
    // Verifica se o caminho fornecido é válido
    fs::File rootDir = openFile(path, path);

    // Se for pasta
    if (rootDir && rootDir.isDirectory()) {
        // Lista os arquivos no diretório
        fs::File file = rootDir.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                fs.remove(file.path());
            }
            file = rootDir.openNextFile();
        }
        // Se for arquivo
    } else if (rootDir) {
        fs.remove(rootDir.path());
    }
}

/// @brief Lista arquivos do diretório
/// @param path caminho do diretório
cJSON *MyFS::listFiles(const char *path) { // Cria o objeto JSON
    cJSON *root = cJSON_CreateObject();
    cJSON *files = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "files", files);

    // Verifica se o caminho fornecido é válido
    fs::File rootDir = fs.open(path);
    if (rootDir && rootDir.isDirectory()) {
        // Lista os arquivos no diretório
        fs::File file = rootDir.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                cJSON_AddItemToArray(files, cJSON_CreateString(file.name()));
            }
            file = rootDir.openNextFile();
        }
    }

    return root;
}

/// @brief counts how many logs were saved
/// @param path path to archives directory
/// @return int, number of files
int MyFS::numberOfFiles(const char *path) {
    int num = 0;

    // Verifica se o caminho fornecido é válido
    fs::File rootDir = openFile(path, true, path);
    if (rootDir && rootDir.isDirectory()) {
        // Lista os arquivos no diretório
        fs::File file = rootDir.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                num++;
            }
            file = rootDir.openNextFile();
        }
    } else {
        return 0;
    }
    return num;
}

/// @brief Grava string em um arquivo
/// @param path caminho do diretório
/// @param content texto string para ser gravado
/// @param create indica se é um novo arquivo [default = false]
bool MyFS::writeFile(const char *path, const char *content, bool create) {
    fs::File file = fs.open(path, "w", create);
    if (!file) {
        // SERIAL_OUT.println("- failed to open file for writing");
        return false;
    }
    if (file.print(content)) {
        // SERIAL_OUT.println("- file written");
        return true;
    } else {
        // SERIAL_OUT.println("- write failed");
        return false;
    }
    file.close();
}

/// @brief Grava string em um arquivo
/// @attention Não abre nem fecha o arquivo 'file'
/// @param file ponteiro para o arquivo
/// @param message texto string para ser gravado
/// @param create indica se é um novo arquivo [default = false]
bool MyFS::writeFile(File file, const char *message) {
    if (!file) {
        SERIAL_OUT.println("- failed to open file for writing");
        return false;
    }
    if (file.print(message)) {
        // SERIAL_OUT.println("- file written");
        return true;
    } else {
        SERIAL_OUT.println("- write failed");
        return false;
    }
}

/// @brief Cria um diretório
/// @attention deve fechar o arquivo depois
/// @param filePath caminho do arquivo de retorno
/// @param write define o modo de abertura [true - WRITE, false - READ], e se o diretório/arquivo será criado
/// @param dirPath caminho do diretório que será criado
File MyFS::openFile(const char *filePath, bool write, const char *dirPath) {
    // cria diretório se necessário
    if (write) {
        if (!fs.exists(dirPath)) {
            if (fs.mkdir(dirPath)) {
                SERIAL_OUT.print("Dir created: ");
                SERIAL_OUT.println(filePath);
            } else {
                SERIAL_OUT.println("mkdir failed");
            }
        }

        return fs.open(filePath, FILE_WRITE, true);
        // somente abre o arquivo
    } else {
        return fs.open(filePath, FILE_READ);
    }
}

MyFS myFS;

#include "lib_fs.h"

lib_fs::lib_fs() {}

/// @brief Inicializa LittleFS
/// @return retorna se foi inicializado corretamente
bool lib_fs::begin() {
    if (LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        fs = LittleFS;
        return true;
    } else {
        return false;
    }
}

/// @brief Inicializa LittleFS
/// @note faz o mesmo que myFS.begin()
/// @return retorna se foi inicializado corretamente
bool lib_fs::LittleFSinit() {
    return begin();
}

/// @brief Inicializa SD card
/// @param cs Número do Chip select
/// @param com configuração SPI
/// @return retorna se o SD está montado no sistema
bool lib_fs::SDinit(uint8_t cs, SPIClass &com) {
    if (SD.begin(cs, com)) {
        fs = SD;
        return true;
    } else {
        return false;
    }
}

std::string lib_fs::readFile(const char *path, bool debug) {
    std::string fileContent = "";

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

/// @brief Lê o conteúdo de um arquivo
/// @attention Não fecha o arquivo 'file'
/// @param file ponteiro para o arquivo
std::string lib_fs::readFile(fs::File file, bool debug) {
    std::string fileContent = "";

    if (!file || file.isDirectory()) {
        SERIAL_OUT.println("\n- failed to open file for reading\n");
        return fileContent;
    }
    while (file.available()) {
        char c = file.read();
        fileContent += c;
    }

    if (debug) {
        SERIAL_OUT.println(fileContent.c_str());
    }
    return fileContent;
}

/// @brief imprime na tela os diretórios de uma pasta
/// @param dirname caminho do diretório
/// @param levels quantidade de níveis que serão mostrados
void lib_fs::printDir(const char *dirname, uint8_t levels) {
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
            SERIAL_OUT.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday,
                              tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

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
            SERIAL_OUT.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday,
                              tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

/// @brief salva em jSON os diretórios de uma pasta
/// @attention não testado ainda
/// @param dirname caminho do diretório
/// @param levels quantidade de níveis que serão mostrados
cJSON *lib_fs::listDir(const char *dirname, uint8_t levels) {
    // Cria o objeto JSON raiz
    cJSON *rootJson = cJSON_CreateObject();
    cJSON *filesArray = cJSON_CreateArray();
    cJSON_AddStringToObject(rootJson, "directory", dirname);
    cJSON_AddItemToObject(rootJson, "contents", filesArray);

    fs::File root = fs.open(dirname);
    if (!root) {
        SERIAL_OUT.println("- failed to open directory");
        return rootJson;
    }
    if (!root.isDirectory()) {
        SERIAL_OUT.println(" - not a directory");
        return rootJson;
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
        char lastWrite[20];
        snprintf(lastWrite, sizeof(lastWrite), "%04d-%02d-%02d %02d:%02d:%02d", tmstruct->tm_year + 1900, tmstruct->tm_mon + 1, tmstruct->tm_mday,
                 tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

        cJSON_AddStringToObject(fileEntry, "lastWrite", lastWrite);
        cJSON_AddItemToArray(filesArray, fileEntry);

        file = root.openNextFile();
    }

    return rootJson;
}

/// @brief deletes files of path folder
/// @param path caminho do arquivo ou diretório [deleta todos os arquivos]
void lib_fs::removeFile(const char *path) {
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
cJSON *lib_fs::listFiles(const char *path) { // Cria o objeto JSON
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
int lib_fs::numberOfFiles(const char *path) {
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
bool lib_fs::writeFile(const char *path, const char *content, bool create) {
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
bool lib_fs::writeFile(File file, const char *message) {
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
/// @param write modo de abertura [true - CREATE AND WRITE, false - ONLY READ]
/// @param dirPath caminho do diretório que será criado
File lib_fs::openFile(const char *filePath, bool write, const char *dirPath) {
    if (write) {
        if (strcmp(dirPath, "") != 0) createDir(dirPath);
        return fs.open(filePath, FILE_WRITE, true);
    } else {
        return fs.open(filePath, FILE_READ);
    }
}

/// @brief Cria o diretório no caminho path
/// @param path caminho do diretório
/// @return retorna se true se a pasta foi criada ou se já existe
bool lib_fs::createDir(const char *path) {
    if (fs.exists(path)) {
        return true;
    }

    if (fs.mkdir(path)) {
        SERIAL_OUT.printf("Dir created: %s\n", path);
        return true;
    } else {
        SERIAL_OUT.println("mkdir failed");
        return false;
    }
}

/// @brief Retorna a quantidade de bytes total na memória flash
/// @warning Somente LittleFS
uint lib_fs::getTotalBytes() {
    return LittleFS.totalBytes();
}

/// @brief Retorna a quantidade de bytes usados na memória flash
/// @warning Somente LittleFS
uint lib_fs::getUsedBytes() {
    return LittleFS.usedBytes();
}

/// @brief Retorna a quantidade de bytes livres na memória flash
/// @warning Somente LittleFS
uint lib_fs::getFreeBytes() {
    return (LittleFS.totalBytes() - LittleFS.usedBytes());
}

lib_fs myFS;

#ifndef MYFS_H
#define MYFS_H

#include <FS.h>
#include <LittleFS.h>
#include <SD.h>
#include <cJSON.h>

#include "Arduino.h"

#define FORMAT_LITTLEFS_IF_FAILED true
#define stdstring std::string

#ifdef SERIAL_DEBUG
#define SERIAL_OUT SERIAL_DEBUG
#else
#define SERIAL_OUT Serial0
#endif

class MyFS {
   private:
    fs::FS &fs = LittleFS;

   public:
    MyFS();
    bool LittleFSinit();
    bool SDinit(uint8_t cs, SPIClass &com);
    stdstring readFile(const char *path, bool debug = false);
    void printDir(const char *dirname, uint8_t levels);
    cJSON *listDir(const char *dirname, uint8_t levels);
    cJSON *listFiles(const char *path);
    bool writeFile(const char *path, const char *message, bool create = false);
    bool writeFile(File file, const char *message);
    int numberOfFiles(const char *path);
    File openFile(const char *filePath, bool write = false, const char *dirPath = "");
    void removeFile(const char *path);
};

extern MyFS myFS;

#endif // MYFS_H
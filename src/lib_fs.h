#ifndef MYFS_H
#define MYFS_H

#include <conf.h>
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <SD.h>
#include <cJSON.h>

#define FORMAT_LITTLEFS_IF_FAILED true


class lib_fs {
   private:
    fs::FS &fs = LittleFS;

   public:
    lib_fs();
    bool begin();
    bool LittleFSinit();
    bool SDinit(uint8_t cs, SPIClass &com);
    std::string readFile(const char *path, bool debug = false);
    std::string readFile(fs::File file, bool debug = false);
    void printDir(const char *dirname, uint8_t levels);
    cJSON *listDir(const char *dirname, uint8_t levels);
    cJSON *listFiles(const char *path);
    bool writeFile(const char *path, const char *message, bool create = false);
    bool writeFile(File file, const char *message);
    int numberOfFiles(const char *path);
    File openFile(const char *filePath, bool write = false, const char *dirPath = "");
    void removeFile(const char *path);
    bool createDir(const char *path);
    uint getTotalBytes();
    uint getUsedBytes();
    uint getFreeBytes();
};

extern lib_fs myFS;

#endif // MYFS_H
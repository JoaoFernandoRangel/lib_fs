#ifndef MYFS_H
#define MYFS_H

#include "Arduino.h"
#include <cJSON.h>
#include <SD.h>
#include <LittleFS.h>
#include <FS.h>

#define FORMAT_LITTLEFS_IF_FAILED true
#define string std::string

#ifdef SERIAL_DEBUG
#define SERIAL_OUT SERIAL_DEBUG
#else
#define SERIAL_OUT Serial0
#endif

class MyFS
{
private:
    fs::FS &fs;
public:    
    //MyFS();
    bool MyFS::LittleFSinit();
    bool SDinit(uint8_t cs, SPIClass &com);
    string readFile( const char *path, bool debug);
    void printDir( const char *dirname, uint8_t levels);
    cJSON* listDir(const char *dirname, uint8_t levels);
    cJSON* listFiles(const char *path);
    bool writeFile( const char *path, const char *message, bool create = false);
    bool writeFile( File file, const char *message);
    int numberOfFiles( const char *path);
    File openFile(const char * filePath, bool write = false, const char * dirPath = "");
    void removeFile(const char *path);
};

extern MyFS myFS;

#endif // MYFS_H
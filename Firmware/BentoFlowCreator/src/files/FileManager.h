#pragma once

#include "../common/Common.h"

#include <SPI.h>
#include <SD.h>

class FileManager : public Component
{
public:
    FileManager();
    ~FileManager() {}

    static SPIClass spiSD;
    static bool sdIsDetected;

    void init();
    void update();

    static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
    static void deleteFileIfExists(String path);
    static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

    bool handleCommand(String command, var *data, int numData) override;
};

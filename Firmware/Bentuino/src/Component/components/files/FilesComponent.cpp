ImplementSingleton(FilesComponent);

fs::FS &FilesComponent::fs = FS_TYPE;

bool FilesComponent::initInternal(JsonObject o)
{
    useInternalMemory = false;

    bool mounted = false;
#ifdef FILES_TYPE_MMC

    NDBG("Init SD MMC");
    if (SD_MMC.begin("/sdcard", true)) // if using ESP32 package 3.x.x
    {
        mounted = true;
    }
#else

    NDBG("Init SD SPI");

    AddIntParamConfig(sdEnPin);
    AddBoolParamConfig(sdEnVal);
    AddIntParamConfig(sdSCK);
    AddIntParamConfig(sdMiso);
    AddIntParamConfig(sdMosi);
    AddIntParamConfig(sdCS);
    AddIntParamConfig(sdSpeed);

    if (sdEnPin > 0)
    {
        // NDBG("Setting SD En Pin " + String(sdEnPin) + " to " + String(sdEnVal));
        pinMode(sdEnPin, OUTPUT);
        digitalWrite(sdEnPin, sdEnVal);
    }

    if (sdSCK == 0 || sdMiso == 0 || sdMosi == 0 || sdCS == 0)
    {
        String npin;
        if (sdSCK == 0)
            npin += "SCK,";
        if (sdMiso == 0)
            npin += "MISO,";
        if (sdMosi == 0)
            npin += "MOSI,";
        if (sdCS == 0)
            npin += "CS";

        NDBG(npin + " pins not defined, using internal memory");
        return initInternalMemory();
    }

    // NDBG("initilializing SD with pins SCK,MISO,MOSI,CS,Speed : " + sdSCK.stringValue() + "," + sdMiso.stringValue() + "," + sdMosi.stringValue() + "," + sdCS.stringValue() + "," + sdSpeed.stringValue());
    pinMode(sdSCK, INPUT_PULLUP);
    pinMode(sdMiso, INPUT_PULLUP);
    pinMode(sdMosi, INPUT_PULLUP);
    pinMode(sdCS, OUTPUT);
    digitalWrite(sdCS, LOW);

    spiSD.begin((int8_t)sdSCK, (int8_t)sdMiso, (int8_t)sdMosi, (int8_t)sdCS); // SCK,MISO,MOSI,ss

    if (SD.begin((uint8_t)sdCS, spiSD))
    {

        //    NDBG("SD Card initialized.");
        //    listDir("/", 1);

        mounted = true;
    }
#endif

    if (mounted)
    {
        NDBG("SD Mounted");
        fs.mkdir("/scripts");
        fs.mkdir("/playback");
        fs.mkdir("/server");
    }
    else
    {
        NDBG("Error initializing SD Card, using internal memory");
        useInternalMemory = true;
    }

    initInternalMemory();
    return true;
}

bool FilesComponent::initInternalMemory()
{
    if (!SPIFFS.begin(true)) // Start the SPI Flash Files System
    {
        NDBG("Error initializing SPIFFS");
        return false;
    }
    NDBG("Internal Memory SPIFFS initialized.");
    return true;
}

File FilesComponent::openFile(String fileName, bool forWriting, bool deleteIfExists)
{

    if (forWriting && deleteIfExists)
        deleteFileIfExists(fileName);

    if (!fileName.startsWith("/"))
        fileName = "/" + fileName;

    File f;
    if (useInternalMemory)
        f = SPIFFS.open(fileName, forWriting ? "w" : "r"); // Open it
    else
        f = fs.open(fileName.c_str(), forWriting ? FILE_WRITE : FILE_READ);

    return f;
}

bool FilesComponent::deleteFolder(String path)
{
    if (!isInit)
        return false;

    bool result = false;

    File dir = fs.open(path);

    File entry;
    while (entry = dir.openNextFile())
    {
        String fPath = entry.path();

        if (entry.isDirectory())
        {
            NDBG("Deleting folder " + fPath);

            result = deleteFolder(fPath);
            if (!result)
            {
                NDBG("Failed to delete folder " + fPath);
                return false;
            }

            if (useInternalMemory)
                result = SPIFFS.rmdir(fPath);
            else
                result = fs.rmdir(fPath);
        }
        else
        {
            NDBG("Deleting file " + fPath);

            if (useInternalMemory)
                result = SPIFFS.remove(fPath);
            else
                result = fs.remove(fPath);
        }
    }

    return result;
}

void FilesComponent::deleteFileIfExists(String path)
{

    if (!isInit)
        return;

    NDBG("Deleting file " + path);

    if (useInternalMemory)
    {

        if (SPIFFS.exists(path))
            SPIFFS.remove(path);
    }
    else
    {

        if (fs.exists(path.c_str()))
            fs.remove(path.c_str());
    }
}

String FilesComponent::listDir(const char *dirname, uint8_t levels)
{
    String result = "";

    File root;

    if (useInternalMemory)
        root = SPIFFS.open("/", "r");
    else
        root = fs.open(dirname);

    if (!root)
    {
        NDBG("Failed to open directory");
        return result;
    }

    if (!root.isDirectory())
    {
        NDBG("Not a directory");
        return result;
    }

    File file = root.openNextFile();
    if (!file)
        NDBG("  [Empty directory]");

    while (file)
    {
        if (file.isDirectory())
        {
            NDBG("  DIR : " + String(file.name()));
            if (levels)
            {
                result += listDir(file.path(), levels - 1);
            }
        }
        else
        {
            NDBG("  FILE: " + String(file.name()));
            result += String(file.path()) + ",";
            NDBG("  SIZE: " + String(file.size()));
        }
        file = root.openNextFile();
    }

    return result;
}

esp_err_t FilesComponent::format_sdcard() {
#ifdef FILES_TYPE_SD
    
    char drv[3] = {'0', ':', 0};
    const size_t workbuf_size = 4096;
    void* workbuf = NULL;
    esp_err_t err = ESP_OK;
    ESP_LOGW("sdcard", "Formatting the SD card");

    size_t allocation_unit_size = 16 * 1024;
    int sector_size_default = 512;

    workbuf = ff_memalloc(workbuf_size);
    if (workbuf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
                sector_size_default,
                allocation_unit_size);

#if (ESP_IDF_VERSION_MAJOR < 5)
    FRESULT res = f_mkfs(drv, FM_ANY, alloc_unit_size, workbuf, workbuf_size);
#else
    const MKFS_PARM opt = {(BYTE)FM_ANY, 0, 0, 0, alloc_unit_size};
    FRESULT res = f_mkfs(drv, &opt, workbuf, workbuf_size);
#endif  /* ESP_IDF_VERSION_MAJOR */
    if (res != FR_OK) {
        err = ESP_FAIL;
        ESP_LOGE("sdcard", "f_mkfs failed (%d)", res);
    }

    free(workbuf);

    ESP_LOGI("sdcard", "Successfully formatted the SD card");

    return err;
#endif
}

bool FilesComponent::handleCommandInternal(const String &command, var *data, int numData)
{

    if (checkCommand(command, "delete", numData, 1))
    {
        deleteFileIfExists(data[0].stringValue());
        return true;
    }
    else if (checkCommand(command, "deleteFolder", numData, 0))
    {
        deleteFolder(numData > 0 ? data[0].stringValue() : "/");
        return true;
    }
    else if (checkCommand(command, "list", numData, 0))
    {
        var filesData;
        filesData.type = 's';
        int level = numData == 1 ? data[0].intValue() : 0;
        filesData = listDir("/", level);

        sendEvent(FileList, &filesData, 1);
        return true;
    }
    else if (checkCommand(command, "format", numData, 0))
    {
        NDBG("Formatting SD card.");
        format_sdcard();
    }

    return false;
}

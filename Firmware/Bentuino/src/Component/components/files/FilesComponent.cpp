ImplementSingleton(FilesComponent);

bool FilesComponent::initInternal(JsonObject o)
{
    useInternalMemory = false;

    AddAndSetParameter(sdEnPin);
    AddAndSetParameter(sdEnVal);
    AddAndSetParameter(sdSCK);
    AddAndSetParameter(sdMiso);
    AddAndSetParameter(sdMosi);
    AddAndSetParameter(sdCS);
    AddAndSetParameter(sdSpeed);

    if (sdEnPin.intValue() > 0)
    {
        // NDBG("Setting SD En Pin " + String(sdEnPin.intValue()) + " to " + String(sdEnVal.intValue()));
        pinMode(sdEnPin.intValue(), OUTPUT);
        digitalWrite(sdEnPin.intValue(), sdEnVal.intValue());
    }

    if (sdSCK.intValue() == 0 || sdMiso.intValue() == 0 || sdMosi.intValue() == 0 || sdCS.intValue() == 0)
    {
        String npin;
        if (sdSCK.intValue() == 0)
            npin += "SCK,";
        if (sdMiso.intValue() == 0)
            npin += "MISO,";
        if (sdMosi.intValue() == 0)
            npin += "MOSI,";
        if (sdCS.intValue() == 0)
            npin += "CS";

        NDBG(npin + " pins not defined, using internal memory");
        return initInternalMemory();
    }

    // NDBG("initilializing SD with pins SCK,MISO,MOSI,CS,Speed : " + sdSCK.stringValue() + "," + sdMiso.stringValue() + "," + sdMosi.stringValue() + "," + sdCS.stringValue() + "," + sdSpeed.stringValue());
    pinMode(sdSCK.intValue(), INPUT_PULLUP);
    pinMode(sdMiso.intValue(), INPUT_PULLUP);
    pinMode(sdMosi.intValue(), INPUT_PULLUP);
    pinMode(sdCS.intValue(), OUTPUT);
    digitalWrite(sdCS.intValue(), LOW);

    spiSD.begin((int8_t)sdSCK.intValue(), (int8_t)sdMiso.intValue(), (int8_t)sdMosi.intValue(), (int8_t)sdCS.intValue()); // SCK,MISO,MOSI,ss

    if (SD.begin((uint8_t)sdCS.intValue(), spiSD))
    {

        //    NDBG("SD Card initialized.");
        SD.mkdir("/scripts");
        SD.mkdir("/sequences");
        SD.mkdir("/bake");
        SD.mkdir("/server");
        //    listDir("/", 1);
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
    // NDBG("SPIFFS initialized.");
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
        f = SD.open(fileName.c_str(), forWriting ? FILE_WRITE : FILE_READ);

    return f;
}

void FilesComponent::deleteFileIfExists(String path)
{

    if (!isInit)
        return;

    if (useInternalMemory)
    {
        if (SPIFFS.exists(path))
            SPIFFS.remove(path);
    }
    else
    {

        if (SD.exists(path.c_str()))
            SD.remove(path.c_str());
    }
}

String FilesComponent::listDir(const char *dirname, uint8_t levels)
{
    String result = "";

    File root;

    if (useInternalMemory)
        root = SPIFFS.open("/", "r");
    else
        root = SD.open(dirname);

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
                result += listDir(file.name(), levels - 1);
            }
        }
        else
        {
            NDBG("  FILE: " + String(file.name()));
            result += String(file.name()) + ",";
            NDBG("  SIZE: " + String(file.size()));
        }
        file = root.openNextFile();
    }

    return result;
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
        if (numData > 0)
        {
            NDBG("Deleting folder " + data[0].stringValue());
#ifdef FILES_USE_INTERNAL_MEMORY
            SPIFFS.rmdir(data[0].stringValue());
#else
            SD.rmdir(data[0].stringValue());
#endif
        }
        else
        {
            NDBG("Deleting all files");
#ifdef FILES_USE_INTERNAL_MEMORY
            SPIFFS.rmdir("/");
#else
            SD.rmdir("/");
#endif
        }

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

    return false;
}
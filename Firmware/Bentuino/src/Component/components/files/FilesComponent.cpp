ImplementSingleton(FilesComponent);

bool FilesComponent::initInternal(JsonObject o)
{
    useInternalMemory = false;

    AddIntParam(sdEnPin);
    AddBoolParam(sdEnVal);
    AddIntParam(sdSCK);
    AddIntParam(sdMiso);
    AddIntParam(sdMosi);
    AddIntParam(sdCS);
    AddIntParam(sdSpeed);

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
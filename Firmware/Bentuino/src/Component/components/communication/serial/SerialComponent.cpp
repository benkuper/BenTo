ImplementSingleton(SerialComponent);

bool SerialComponent::initInternal(JsonObject o)
{
    bufferIndex = 0;
    memset(buffer, 0, 512);
    Serial.begin(115200);
    feedbackEnabled = AddParameter("feedbackEnabled", false);
    return true;
}

void SerialComponent::updateInternal()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            bufferIndex = 0;
            processMessage(buffer);
            memset(buffer, 0, 512);
        }
        else
        {
            if (bufferIndex < 512)
                buffer[bufferIndex] = c;
            bufferIndex++;
        }
    }
}

void SerialComponent::clearInternal()
{
}

void SerialComponent::processMessage(String buffer)
{
    if (buffer.substring(0, 2) == "yo")
    {
        Serial.println("wassup " + DeviceID + " \"" + String(ARDUINO_BOARD) + "\"");
        return;
    }

    int splitIndex = buffer.indexOf(' ');

    String target = buffer.substring(0, splitIndex); // target component

    int tcIndex = target.lastIndexOf('.');

    String tc = target.substring(0, tcIndex);                                 // component name
    String cmd = target.substring(tcIndex + 1);                               // parameter name
    String args = (splitIndex != -1 ? buffer.substring(splitIndex + 1) : ""); // value

    const int numData = 10;    
    var data[numData];

    data[0] = tc;
    data[1] = cmd;

    int index = 2;
    // COUNT
    char *pch = strtok((char *)args.c_str(), ",");
    while (pch != NULL && index < numData)
    {
        String s = String(pch);

        bool isNumber = true;
        for (byte i = 0; i < s.length(); i++)
        {
            char c = s.charAt(i);
            if (c != '.' && c != '-' && c != '+' && !isDigit(c))
            {
                isNumber = false;
                break;
            }
        }

        if (isNumber)
        {
            float f = s.toFloat();
            int i = s.toInt();

            if (f == i && s.indexOf('.') == -1)
                data[index] = i;
            else
                data[index] = f;
        }
        else
        {
            data[index] = pch;
        }

        pch = strtok(NULL, ",");
        index++;
    }

    sendEvent(MessageReceived, data, index);

    //free(data);
}

void SerialComponent::sendMessage(String source, String command, var *data, int numData)
{
    if (!feedbackEnabled->boolValue())
        return;

    String msg = source + "." + command;
    for (int i = 0; i < numData; i++)
    {
        msg += " " + data[i].stringValue();
    }

    Serial.println(msg);
}

void SerialComponent::send(const String &message)
{
    Serial.println(message);
}
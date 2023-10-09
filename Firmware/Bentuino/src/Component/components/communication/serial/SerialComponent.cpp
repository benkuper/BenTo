ImplementSingleton(SerialComponent);

bool SerialComponent::initInternal(JsonObject o)
{
    bufferIndex = 0;
    memset(buffer, 0, 512);
    Serial.begin(115200);
    AddAndSetParameter(sendFeedback);
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

    ParsingHelper::processStringMessage(buffer, [this](var *data, int numdata)
                                        { sendEvent(MessageReceived, data, numdata); });

    // free(data);
}

void SerialComponent::sendMessage(String source, String command, var *data, int numData)
{
    if (!sendFeedback.boolValue())
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
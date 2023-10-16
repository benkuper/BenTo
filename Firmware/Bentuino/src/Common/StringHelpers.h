#pragma once

class StringHelpers
{
public:
    static void processStringMessage(const String &buffer, std::function<void(var *data, int numData)> callback);

    static String lowerCamelToTitleCase(String input);

    static String ipToString(IPAddress ip);
};
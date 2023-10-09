#pragma once

class ParsingHelper
{
public:
    static void processStringMessage(const String &buffer, std::function<void(var *data, int numData)> callback);
};
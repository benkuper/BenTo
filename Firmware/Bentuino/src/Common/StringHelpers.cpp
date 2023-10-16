#include "StringHelpers.h"

void StringHelpers::processStringMessage(const String &buffer, std::function<void(var *data, int numData)> callback)
{
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

    callback(data, index);
}
String StringHelpers::lowerCamelToTitleCase(String input)
{
    String result = "";
    bool wasSpecial = false;

    for (int i = 0; i < input.length(); i++)
    {
        char c = input.charAt(i);
        String s = input.substring(i, i + 1);

        bool up = isUpperCase(c);
        bool isNumber = isDigit(c);

        if (up || isNumber)
        {
            if (!wasSpecial)
                result += " ";
            result += c;
        }
        else if (i == 0)
        {
            result += (char)toUpperCase(c);
        }
        else
        {
            result += c;
        }

        wasSpecial = up || isNumber;
    }

    return result;
}

String StringHelpers::ipToString(IPAddress ip)
{
    return String(ip[0]) +
           "." + String(ip[1]) +
           "." + String(ip[2]) +
           "." + String(ip[3]);
}

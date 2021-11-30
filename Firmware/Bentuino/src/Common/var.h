struct var
{
    char type;
    union
    {
        int i;
        float f;
        char * s;
    } value;

    int intValue() const
    {
        switch (type)
        {
        case 'i':
            return value.i;
        case 'f':
            return (int)value.f;
        case 's':
            return String(value.s).toInt();
        }
        return 0;
    }

    float floatValue() const
    {
        switch (type)
        {
        case 'i':
            return (float)value.i;
        case 'f':
            return value.f;
        case 's':
            return String(value.s).toFloat();
        }

        return 0;
    }

    String stringValue() const
    {
        switch (type)
        {
        case 'i':
            return String(value.i);
        case 'f':
            return String(value.f);
        case 's':
            return value.s;
        }
        return "";
    }
};

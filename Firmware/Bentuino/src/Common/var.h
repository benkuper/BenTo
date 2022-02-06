struct var
{
    char type;
    union
    {
        bool b;
        int i;
        float f;
        char *s;
    } value;

    var() {type = '?';}

    var(bool v)
    {
        type = 'b';
        value.b = v;
    };
    var(int v)
    {
        type = 'i';
        value.i = v;
    };
    var(float v)
    {
        type = 'f';
        value.f = v;
    };

    var(String v)
    {
        type = 's';
        value.s = (char *)v.c_str();
    };

    var(char * v)
    {
        type = 's';
        value.s = v;
    };

    
    operator bool() const { return boolValue(); }
    operator int() const { return intValue(); }
    operator float() const { return floatValue(); }
    operator String() const { return stringValue(); }
    operator char *() const { return value.s; }

    bool isVoid() { return type == '?'; }    

    int boolValue() const
    {
        switch (type)
        {
        case 'b':
            return value.b;
        case 'i':
            return value.i;
        case 'f':
            return (int)value.f;
            
        case 's':
            return String(value.s).toInt();
        }
        return 0;
    }

    int intValue() const
    {
        switch (type)
        {
        case 'b':
            return (int)value.b;

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
        case 'b':
            return (float)value.b;
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
        case 'b':
            return String((int)value.b);
        case 'i':
            return String(value.i);
        case 'f':
            return String(value.f);
        case 's':
            return value.s;
        }
        return "";
    }

    bool operator== (const var &other) const
    {
        switch (type)
        {
        case 'b':
            return boolValue() == other.boolValue();
        case 'i':
            return intValue() == other.intValue();
        case 'f':
            return floatValue() == other.floatValue();
        case 's':
            return stringValue() == other.stringValue();
        }

        return false;
    }
};

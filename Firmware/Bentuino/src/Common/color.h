struct Color
{
    union
    {
        struct
        {
            uint8_t a;
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint8_t raw[4];
        uint32_t value;
    };

    Color operator+(Color &c) const { return Color(min<uint8_t>(r + c.r, 255), min<uint8_t>(g + c.g, 255), min<uint8_t>(b + c.b, 255), min<uint8_t>(a + c.a, 255)); }
    Color operator+(uint8_t &val) const { return Color(min<uint8_t>(r + val, 255), min<uint8_t>(g + val, 255), min<uint8_t>(b + val, 255), min<uint8_t>(a + val, 255)); }
    Color operator-(Color &c) const { return Color(max<uint8_t>(r - c.r, 0), max<uint8_t>(g - c.g, 0), max<uint8_t>(b - c.b, 0), max<uint8_t>(a - c.a, 0)); }
    Color operator-(uint8_t &val) const { return Color(max<uint8_t>(r - val, 0), max<uint8_t>(g - val, 0), max<uint8_t>(b - val, 0), max<uint8_t>(a - val, 0)); }

    Color operator*(const float &val) const { return Color(min<uint8_t>(max<uint8_t>((uint8_t)(r * val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(g * val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(b * val), 0), 255),min<uint8_t>(max<uint8_t>((uint8_t)(a * val), 0), 255)); }
    Color operator/(const float &val) const { return Color(min<uint8_t>(max<uint8_t>((uint8_t)(r / val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(g / val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(b / val), 0), 255),min<uint8_t>(max<uint8_t>((uint8_t)(a / val), 0), 255)); }

    Color(uint8_t r = 0x00, uint8_t g = 0x00, uint8_t b = 0x00, uint8_t a = 0xff)
    {
        value = a << 24 | r << 16 | g << 8 | b;
    }

    Color(float h, float s, float v)
    {
        value = a << 24 | r << 16 | g << 8 | b;
    }

    Color lerp(Color a, Color b, float weight)
    {
        Color r;
        for (uint8_t i = 0; i < 4; i++)
        {
            if (a.raw[i] == b.raw[i])
                r.raw[i] = a.raw[i];
            else
                r.raw[i] = a.raw[i] + weight * (b.raw[i] - a.raw[i]);
        }

        return r;
    }
};
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

    inline Color operator+(Color &c) const
    {
        float ta = c.a / 255.0f;
        return Color(r + min<uint8_t>(c.r * ta, 255 - r), min<uint8_t>(c.g * ta, 255 - g), min<uint8_t>(c.b * ta, 255 - b), a + min<uint8_t>(c.a, 255 - a));
    }
    inline Color operator-(Color &c) const { return Color(max(r - c.r, 0), max(g - c.g, 0), max(b - c.b, 0), max(a - c.a, 0)); }

    inline Color operator*(Color &c) const { return Color(min<uint8_t>(max<uint8_t>((uint8_t)(r * (c.r / 255.0f)), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(g * (c.g / 255.0f)), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(b * (c.b / 255.0f)), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(a * (c.a / 255.0f)), 0), 255)); }
    inline Color operator*(const float &val) const { return Color(min<uint8_t>(max<uint8_t>((uint8_t)(r * val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(g * val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(b * val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(a * val), 0), 255)); }
    inline Color operator/(const float &val) const { return Color(min<uint8_t>(max<uint8_t>((uint8_t)(r / val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(g / val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(b / val), 0), 255), min<uint8_t>(max<uint8_t>((uint8_t)(a / val), 0), 255)); }

    inline Color &operator+=(Color &c)
    {
        float ta = c.a / 255.0f;
        r += min<uint8_t>(c.r * ta, 255 - r);
        g += min<uint8_t>(c.g * ta, 255 - g);
        b += min<uint8_t>(c.b * ta, 255 - b);
        a += min<uint8_t>(c.a, 255 - a);
        return *this;
    }
    inline Color &operator+=(const uint8_t &val)
    {
        r = min(r + val, 255);
        g = min(g + val, 255);
        b = min(b + val, 255);
        a = min(a + val, 255);
        return *this;
    }
    inline Color &operator-=(Color &c)
    {
        r = max(r - c.r, 0);
        g = max(g - c.g, 0);
        b = max(b - c.b, 0);
        a = max(a - c.a, 0);
        return *this;
    }
    inline Color &operator-=(const uint8_t &val)
    {
        r = max(r - val, 0);
        g = max(g - val, 0);
        b = max(b - val, 0);
        a = max(a - val, 0);
        return *this;
    }
    inline Color &operator*=(Color &c)
    {
        r = min<uint8_t>(max<uint8_t>(r * c.r, 0), 255);
        g = min<uint8_t>(max<uint8_t>(g * c.g, 0), 255);
        b = min<uint8_t>(max<uint8_t>(b * c.b, 0), 255);
        a = min<uint8_t>(max<uint8_t>(a * c.a, 0), 255);
        return *this;
    }
    inline Color &operator*=(const uint8_t &val)
    {
        r = min<uint8_t>(max<uint8_t>(r * val, 0), 255);
        g = min<uint8_t>(max<uint8_t>(g * val, 0), 255);
        b = min<uint8_t>(max<uint8_t>(b * val, 0), 255);
        a = min(a + val, 255);
        return *this;
    }

    Color(uint8_t r = 0x00, uint8_t g = 0x00, uint8_t b = 0x00, uint8_t a = 0xff) : a(a), r(r), g(g), b(b)
    {
    }

    Color clone() { return Color(r, g, b, a); }

    static Color HSV(float h, float s, float v, float a = 1.0f)
    {
        return Color(
            v * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s) * 255,
            v * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s) * 255,
            v * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s) * 255,
            a * 255);
    }

    static float mix(float a, float b, float t) { return a + (b - a) * t; }
    static float fract(float x) { return x - int(x); }

    Color withMultipledAlpha(float val)
    {
        Color c = clone();
        c.a *= val;
        return c;
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

    String toString() const { return "[" + String(r) + "," + String(g) + "," + String(b) + "," + String(a) + "]"; }
};
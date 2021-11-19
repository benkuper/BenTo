struct Color
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

    Color(uint8_t r = 0x00, uint8_t g = 0x00, uint8_t b = 0x00, uint8_t a = 0xff)
    {
        value = a << 24 | r << 16 | g << 8 | b;
    }

    Color(float h, float s, float v)
    {
        value = a << 24 | r << 16 | g << 8 | b;
    }
};
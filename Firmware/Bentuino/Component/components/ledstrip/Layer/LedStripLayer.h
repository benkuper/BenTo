class LedStripComponent;

class LedStripLayer :
    public Component
{
public:
    enum Type
    {
        SYSTEM,
        STREAM,
        BAKE,
        SCRIPT
    };

    enum BlendMode
    {
        ADD,
        MULTIPLY,
        MAX,
        MIN,
        ALPHA
    };

    LedStripLayer(const String &name, Type t, LedStripComponent * strip);
    ~LedStripLayer();

    LedStripComponent * strip;
    int numLeds;
    Type type;
    BlendMode blendMode;
    
    Color * colors;
};
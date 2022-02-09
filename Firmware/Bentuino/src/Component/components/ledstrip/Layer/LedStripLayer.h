class LedStripComponent;

class LedStripLayer :
    public Component
{
public:
    enum Type
    {
        System,
        Stream,
        Bake,
        Script
    };

    enum BlendMode
    {
        Add,
        Multiply,
        Max,
        Min,
        Alpha
    };

    LedStripLayer(const String &name, Type t, LedStripComponent * strip);
    ~LedStripLayer();

    void initColors();

    LedStripComponent * strip;
    int numLeds;
    Type type;
    Parameter * blendMode;
    
    Color * colors;

    //Helper functions
    void clearColors();
    void fillAll(Color c);
    void fillRange(Color c, float start, float end, bool clear = true);
    void point(Color c, float pos, float radius, bool clear = true);
};
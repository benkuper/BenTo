#include "../../common/Common.h"

class IRLedsManager : public Component {
public:
    IRLedsManager();
    ~IRLedsManager();

    float brightness;

    void init();
    void update();

    void setBrightness(float value);
    bool handleCommand(String command, var * data, int numData) override;
};
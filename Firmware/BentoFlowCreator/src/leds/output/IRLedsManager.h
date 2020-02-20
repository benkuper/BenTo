#include "../../common/Common.h"

class IRLedsManager : public Component {
public:
    IRLedsManager();
    ~IRLedsManager();

    const int maxBrightness = 60;

    void setBrightness(float value);
    bool handleCommand(String command, var * data, int numData) override;
};
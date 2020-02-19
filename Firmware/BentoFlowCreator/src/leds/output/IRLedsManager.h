#include "../../Common.h"

class IRLedManager : public Component {
public:
    IRLedsManager();
    ~IRLedsManager();

    const int maxBrightness = 60;

    void setBrightness(float value);
    void handleCommand(String command, data * data, int numData) override;
};
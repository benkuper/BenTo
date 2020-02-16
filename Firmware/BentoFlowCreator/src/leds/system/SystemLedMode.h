#include "../../common/Common.h"

class SystemLedMode :
    public Component
{
public:
    SystemLedMode();
    ~SystemLedMode() {}

    bool handleCommand(String command, var * data, int numData) override;
};
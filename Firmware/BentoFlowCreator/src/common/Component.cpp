#include "Component.h"
int Component::numRegisteredComponents = 0;
Component *Component::registeredComponents[64];

Component::Component(const String &name) : name(name)
{
    registeredComponents[numRegisteredComponents++] = this;
}

Component *Component::getComponentForName(String _name)
{
    for (int i = 0; i < numRegisteredComponents; i++)
        if (registeredComponents[i]->name == _name)
            return registeredComponents[i];

    return nullptr;
};

bool Component::checkCommand(const String &command, const String &checkCommand, int numData, int expected)
{
    if(command != checkCommand) return false;
    if(numData >= expected) return true;
    NDBG(command+" expects "+expected+" arguments, "+numData+" given");
    return false;
}

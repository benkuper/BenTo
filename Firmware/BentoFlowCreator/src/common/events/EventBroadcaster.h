#pragma once
#include <functional>

template <class T>
class EventBroadcaster
{
public:
    EventBroadcaster();
    virtual ~EventBroadcaster() {}

    typedef std::function<void(T)> onEvent;
    onEvent listeners[8];
    void addListener(onEvent func);

protected:
    virtual void sendEvent(T data = T());

private:
    int numListeners;
};

template <class T>
EventBroadcaster<T>::EventBroadcaster() : numListeners(0)
{
}

template <class T>
void EventBroadcaster<T>::sendEvent(T data)
{
    for (int i = 0; i < numListeners; i++)
    {
        listeners[i](data);
    }
}

template <class T>
void EventBroadcaster<T>::addListener(onEvent func)
{
    if (numListeners == 8)
        return;

    
    listeners[numListeners] = func;
    numListeners++;
}

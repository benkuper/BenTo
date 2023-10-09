#pragma once
#include <functional>

#define MAX_LISTENERS 8

template <class T>
class EventBroadcaster
{
public:
    EventBroadcaster();
    virtual ~EventBroadcaster() {}

    typedef std::function<void(const T &)> onEvent;
    onEvent listeners[MAX_LISTENERS];
    bool availableListeners[MAX_LISTENERS];
    int addListener(onEvent func);
    void removeListener(int index);

protected:
    virtual void sendEvent(const T &data = T());
};

template <class T>
EventBroadcaster<T>::EventBroadcaster()
{
    for (int i = 0; i < MAX_LISTENERS; i++)
    {
        listeners[i] = nullptr;
        availableListeners[i] = true;
    }
}

template <class T>
void EventBroadcaster<T>::sendEvent(const T &data)
{
    for (int i = 0; i < MAX_LISTENERS; i++)
    {
        if (!availableListeners[i] && listeners[i] != nullptr)
            listeners[i](data);
    }
}

template <class T>
int EventBroadcaster<T>::addListener(onEvent func)
{
    for (int i = 0; i < MAX_LISTENERS; i++)
    {
        if (!availableListeners[i])
            continue;

        listeners[i] = func;
        availableListeners[i] = false;
        return i;
    }

    return -1;
}

template <class T>
void EventBroadcaster<T>::removeListener(int index)
{
    if (index < 0 && index >= MAX_LISTENERS)
        return;

    listeners[index] = nullptr;
    availableListeners[index] = true;
}


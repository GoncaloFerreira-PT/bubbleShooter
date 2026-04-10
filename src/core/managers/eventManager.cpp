#include "core/managers/eventManager.h"

void EventManager::Subscribe(const std::string &event, std::function<void(std::any)> callback)
{
    if(listeners.find(event) == listeners.end())
    {
        listeners[event] = std::vector<EventListener>();
    }
    listeners[event].push_back(EventListener{callback});
}


void EventManager::Unsubscribe(const std::string &event, std::function<void(std::any)> callback)
{
    if(listeners.find(event) == listeners.end()) return;
    //TODO finish this
}


void EventManager::Emit(const std::string &event, std::any data)
{
    if (listeners.find(event) != listeners.end())
    {
        for (EventListener &listener : listeners[event])
        {
            listener.callback(data);
        }
    }
}

#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <SDL3/SDL.h>
#include <any>


struct EventListener{
    std::function<void(std::any)> callback;
};

class EventManager
{
public:

    static EventManager &Instance()
    {
        static EventManager instance;
        return instance;
    }

    void Subscribe(const std::string& event, std::function<void(std::any)> callback);
    void Unsubscribe(const std::string& event, std::function<void(std::any)> callback);
    void Emit(const std::string& event, std::any data = {});

private:
    EventManager() {}
    std::unordered_map<std::string, std::vector<EventListener>> listeners;
};
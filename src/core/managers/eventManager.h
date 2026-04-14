#pragma once
#include <SDL3/SDL.h>
#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct EventListener {
  void *subscriber;
  std::function<void(std::any)> callback;
};

class EventManager {
public:
  static EventManager &Instance() {
    static EventManager instance;
    return instance;
  }

  void Subscribe(const std::string &event, void *subscriber, std::function<void(std::any)> callback);
  void UnsubscribeAll(void *subscriber);
  void Emit(const std::string &event, std::any data = {});

private:
  EventManager() {}

  std::unordered_map<std::string, std::vector<EventListener>> listeners;
};
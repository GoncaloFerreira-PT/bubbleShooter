#include "core/managers/eventManager.h"
#include <algorithm>

void EventManager::Subscribe(const std::string &event, void *subscriber, std::function<void(std::any)> callback) {
  if (listeners.find(event) == listeners.end()) { listeners[event] = std::vector<EventListener>(); }
  listeners[event].push_back(EventListener{subscriber, callback});
}

void EventManager::UnsubscribeAll(void *subscriber) {
  for (auto &pair : listeners) {
    auto &event_listeners = pair.second;
    event_listeners.erase(
        std::remove_if(event_listeners.begin(), event_listeners.end(),
                       [subscriber](const EventListener &listener) { return listener.subscriber == subscriber; }),
        event_listeners.end());
  }
}

void EventManager::Emit(const std::string &event, std::any data) {
  if (listeners.find(event) != listeners.end()) {
    auto listenersCopy = listeners[event];
    for (EventListener &listener : listenersCopy) {
      listener.callback(data);
    }
  }
}

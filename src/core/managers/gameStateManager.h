#pragma once
#include "core/utils/console.h"
#include <any>
#include <string>
#include <unordered_map>


enum class GameState {
  NONE,
  Menu,
  Game,
  EndGame,
};

class GameStatePayload {
private:
  std::unordered_map<std::string, std::any> data;

public:
  void Clear() { data.clear(); }

  template <typename T> void Set(const std::string &key, T value) { data[key] = value; }

  template <typename T> T Get(const std::string &key, T defaultValue = T{}) const {
    auto it = data.find(key);
    if (it != data.end()) {
      try {
        return std::any_cast<T>(it->second);
      } catch (const std::bad_any_cast &) {
        Console::Warn("GameStatePayload: Type mismatch for key '" + key + "'");
        return defaultValue;
      }
    }
    return defaultValue;
  }

  bool Has(const std::string &key) const { return data.find(key) != data.end(); }
};

class GameStateManager {
private:
  GameState currentState = GameState::NONE;
  GameStatePayload payload;


public:
  static GameStateManager &Instance() {
    static GameStateManager instance;
    return instance;
  }

  void ChangeState(GameState state);

  GameStatePayload *GetPayload() { return &payload; }

  GameState GetCurrentState() { return payload.Get<GameState>("currentState"); }
};

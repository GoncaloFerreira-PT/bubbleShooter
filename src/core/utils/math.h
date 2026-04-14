#pragma once
#include <SDL3/SDL.h>
#include <algorithm> // for std::clamp
#include <cmath>
#include <functional>

namespace Math {
  constexpr float PI = 3.14159265f;

  inline float RadToDeg(float rad) { return rad * (180.0f / PI); }

  inline float DegToRad(float deg) { return deg * (PI / 180.0f); }

  inline float Lerp(float a, float b, float t) { return a + (b - a) * std::clamp(t, 0.0f, 1.0f); }

  inline float InverseLerp(float a, float b, float value) {
    if (std::abs(a - b) < 1e-6f) return 0.0f;
    return std::clamp((value - a) / (b - a), 0.0f, 1.0f);
  }

  inline float MapRange(float inMin, float inMax, float outMin, float outMax, float value) {
    return Lerp(outMin, outMax, InverseLerp(inMin, inMax, value));
  }

  struct vec2 {
    float x, y;

    bool operator==(const vec2 &o) const { return x == o.x && y == o.y; }

    vec2 operator+(const vec2 &o) const { return {x + o.x, y + o.y}; }

    vec2 operator-(const vec2 &o) const { return {x - o.x, y - o.y}; }

    vec2 operator-(const float &o) const { return {x - o, y - o}; }

    vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }

    float length_sq() const { return x * x + y * y; }

    float length() const { return std::sqrt(length_sq()); }

    vec2 normalized() const {
      float l = length();
      return (l > 0.0f) ? vec2{x / l, y / l} : vec2{0.0f, 0.0f};
    }

    float distance_sq(const vec2 &o) const { return (x - o.x) * (x - o.x) + (y - o.y) * (y - o.y); }
  };

  struct vec2i {
    int x, y;

    bool operator==(const vec2i &o) const { return x == o.x && y == o.y; }

    vec2i operator+(const vec2i &o) const { return {x + o.x, y + o.y}; }

    vec2i operator-(const vec2i &o) const { return {x - o.x, y - o.y}; }

    vec2i operator*(int scalar) const { return {x * scalar, y * scalar}; }
  };
} // namespace Math

namespace std {
  template <typename T> struct VecHash {
    size_t operator()(const T &v) const {
      // A better combine hash to prevent collisions
      size_t h1 = hash<decltype(v.x)>{}(v.x);
      size_t h2 = hash<decltype(v.y)>{}(v.y);
      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
  };

  template <> struct hash<Math::vec2> : VecHash<Math::vec2> {};

  template <> struct hash<Math::vec2i> : VecHash<Math::vec2i> {};
} // namespace std
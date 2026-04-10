#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <functional>
#include <cmath>
#include <array>

namespace Math
{
    const float PI = 3.14159265f;

    inline float RadToDeg(float rad)
    {
        return rad * (180.0f / PI);
    }

    inline float DegToRad(float deg)
    {
        return deg * (PI / 180.0f);
    }

    enum class Color : int
    {
        Red,
        Green,
        Blue,
        Yellow,
        COUNT
    };

    inline SDL_Color GetModulateFromEnum(Color col)
    {
        static constexpr std::array<SDL_Color, (size_t)Color::COUNT> colors = {{
            {255, 0, 0, 255},  // Red
            {0, 255, 0, 255},  // Green
            {0, 0, 255, 255},  // Blue
            {255, 255, 0, 255} // Yellow
        }};

        size_t index = static_cast<size_t>(col);
        return (index < colors.size()) ? colors[index] : SDL_Color{255, 255, 255, 255};
    }

    inline Color GetRandomColor()
    {
        return static_cast<Color>(SDL_rand(static_cast<int>(Color::COUNT)));
    }

    inline float Lerp(const float a, const float b, float t)
    {
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;
        return a + (b - a) * t;
    }

    inline float InverseLerp(float a, float b, float value)
    {
        if (a == b)
            return 0.0f;
        float t = (value - a) / (b - a);
        return (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);
    }

    inline float MapRange(float inMin, float inMax, float outMin, float outMax, float value)
    {
        float t = InverseLerp(inMin, inMax, value);
        return Lerp(outMin, outMax, t);
    }

    struct vec2
    {
        float x, y;

        bool operator==(const vec2 &o) const { return x == o.x && y == o.y; }
        vec2 operator+(const vec2 &o) const { return {x + o.x, y + o.y}; }
        vec2 operator-(const vec2 &o) const { return {x - o.x, y - o.y}; }
        vec2 operator-(float o) const { return {x - o, y - o}; }
        vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
        vec2 &operator-=(float o)
        {
            x -= o;
            y -= o;
            return *this;
        }

        float length_sq() const { return x * x + y * y; }
        float length() const { return std::sqrt(length_sq()); }

        vec2 normalized() const
        {
            float l = length();
            return (l > 0) ? vec2{x / l, y / l} : vec2{0, 0};
        }

        float distance_sq(const vec2 &o) const
        {
            return (*this - o).length_sq();
        }
    };

    struct vec2i
    {
        int x, y;

        bool operator==(const vec2i &o) const { return x == o.x && y == o.y; }
        vec2i operator+(const vec2i &o) const { return {x + o.x, y + o.y}; }
        vec2i operator-(const vec2i &o) const { return {x - o.x, y - o.y}; }

        int length_sq() const { return x * x + y * y; }
    };
}

namespace std
{
    template <typename T>
    struct VecHash
    {
        size_t operator()(const T &v) const
        {
            hash<decltype(v.x)> h;
            return h(v.x) ^ (h(v.y) << 1);
        }
    };

    template <>
    struct hash<Math::vec2> : VecHash<Math::vec2>
    {
    };
    template <>
    struct hash<Math::vec2i> : VecHash<Math::vec2i>
    {
    };
}
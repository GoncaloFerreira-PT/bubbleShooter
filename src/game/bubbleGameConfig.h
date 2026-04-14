#pragma once
#include "core/utils/math.h"

namespace Game {
  namespace Config {
    namespace Grid {
      inline constexpr int COLUMNS = 15;
      inline constexpr int ROWS = 5;

      // These are used as an example in the main screen
      inline constexpr int COLUMNS_SHOWCASE = 15;
      inline constexpr int ROWS_SHOWCASE = 10;

      inline constexpr int SPACING_X = 36;
      inline constexpr int SPACING_Y = 36;

      inline constexpr Math::vec2 BALL_SIZE = {32, 32};
    } // namespace Grid

    namespace Rules {
      inline constexpr int NECESSARY_BALLS_TO_DESTROY = 3;
      inline constexpr int BASE_POINTS = 5;
      inline constexpr float COLLIDER_SCALE = 0.7f;
      inline constexpr float BULLET_SPEED = 450.0f;
      inline constexpr float BALL_FALL_SPEED = 150.0f;

      inline constexpr int START_SHOTS_TO_SCROLL = 4;
      inline constexpr int START_BALL_COLORS = 3;
    } // namespace Rules
  } // namespace Config
} // namespace Game
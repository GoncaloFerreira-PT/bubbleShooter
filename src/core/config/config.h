#pragma once

namespace Core {
  namespace Config {
    namespace Window {
      inline constexpr int WIDTH = 640;
      inline constexpr int HEIGHT = 480;
    }

    namespace Physics {
      namespace Raycast {
        inline constexpr float MAX_DISTANCE = 1000.0f;
        inline constexpr float STEP_SIZE = 5.0f;
        inline constexpr float VISUAL_STEP_SIZE = 30.0f;
      }
    }
  } // namespace Config
} // namespace Core
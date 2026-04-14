# 🔵 Bubble Shooter (C++ / SDL3)

A custom-built, cross-platform Bubble Shooter arcade game developed in **C++20** using the **SDL3** library. 

This project was built from scratch to demonstrate advanced C++ concepts, custom game engine architecture, memory management, and modern CMake build systems with automated CI/CD deployment.

!(![Bubble Shooter Gameplay](https://github.com/GoncaloFerreira-PT/bubbleShooter/blob/master/video.gif?raw=true))

---

## 🛠️ Building the Project
Prerequisites:

    Note: This project is currently configured to build exclusively on Windows (x64) using locally provided pre-compiled SDL3 binaries.
    Or Mac using Cmake's `FetchContent`

- A C++20 compatible compiler (MSVC, GCC, or Clang)
- CMake (v3.16+)

Steps:

1. Clone the repository:
    - git clone https://github.com/YourName/BubbleShooter.git

2. Generate the build files:
    - cmake -B build

3. Compile the game:
    - cmake --build build --config Release

4. Run the game:

    - Windows: .\build\Release\BubbleShooter.exe

    - Mac: ./build/BubbleShooter

---
## 🎮 How to Play

The objective of the game is to clear as many bubbles as possible by matching 3 or more of the same color before they reach the bottom of the screen. The game increases its difficulty the more points the player gets, making it an infinite survival style bubble shooter.
The game maintains the highest score to entice the player.

### Controls
* **Mouse Move**: Aim your shot.
* **Left Mouse Click**: Shoot the bubble.
* **`D` Key**: Toggle **Debug View** (Reveals the collision boxes).

---
## 🎨 Assets & Credits
Art & Sprites: Made using Aseprite, with conceptual design assistance from Generative AI tools.

Sound Effects: Synthesized and generated using bfxr.net, capturing the classic arcade feel.

Music: Made with the assistance of Generative AI tools.

---
## 🏗️ Architecture & File Organization

The codebase is engineered with scalability and clean code principles in mind. It strictly decouples the underlying "Engine" mechanics from the specific "Game" implementation.

### The `src/` Directory Split

#### 1. `core/` (The Custom Engine)
This module contains highly reusable, game-agnostic tools that could theoretically be used to build entirely different 2D games.
* **Managers:**
    - `AudioManager` handles loading and playing of audio;
    - `TextureManager` handles loading and drawing of textures, be them individual or atlas based;
    - `FontManager` handles loading and generating textures based on font and fontSizes;
    - `ResourceManager` handles the parsing and redirects the data contained in resources.json to the respective loaders to process the necessary assets;
    - `CollisionManager` handles AABB overlaps;
    - `EventManager` handles EventListeners and Events, creating a generic communication layer between different classes for ease of use;
    - `GameStateManager` handles the gameState and carries its `GameStatePayload` working as a common data structure for the game states to acess and modify;
* **GameScene and Node:** The foundational `Node` class that stays and is processed in the `GameScene`. The latter is responsible for the game loop (Update and Draw) as well as layering between Nodes and the filtering of Inputs.
* **Common extensions of Node:** `Button`,`ToggleButton`,`TextureRect`,`ColorRect`,`Label`.
* **Utils:** Math functions (Vector2, distance algorithms), `Console` logging and simple `Raycast` logic.

#### 2. `game/` (The Bubble Shooter Logic)
This module handles everything specific to the rules of this game.
* **Entities:** `Ball`, `Bullet`.
* **Grid Logic:** `BallController` (A complex class responsible for the movement and logic of the ball grid).
* **Configuration:** `bubbleGameConfig.h` (A centralized header storing all magic numbers like grid spacing, physics collider scales, and difficulty parameters).

### Directory Tree Overview
```text
📦 BubbleShooter
 ├── 📂 libs/              # Local pre-compiled SDL3 binaries (x64) for rapid Windows development
 ├── 📂 resources/         # Game assets (Art & Audio)
 ├── 📂 src/
 │   ├── 📂 core/          # Engine architecture (Managers, Collisions, Events)
 │   └── 📂 game/          # Bubble Shooter specific logic (Balls, Grid, Config)
 └── 📄 CMakeLists.txt     # Hybrid Cross-Platform build script
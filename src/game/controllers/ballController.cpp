#include "game/controllers/ballController.h"
#include "core/managers/audioManager.h"
#include "core/managers/collisionManager.h"
#include "core/managers/eventManager.h"
#include "core/managers/gameStateManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/utils/console.h"
#include "game/bubbleGameConfig.h"
#include "game/player/bullet.h"
#include <cfloat>
#include <memory>
#include <unordered_map>
#include <unordered_set>

enum Direction {
  Up,
  Down,
  Left,
  Right,
  UpLeft,
  UpRight,
  DownLeft,
  DownRight,
};

// Direction offsets for a hexagonal/staggered grid
std::unordered_map<Direction, vec2i> directionVectors = {
    {Direction::Left, {-1, 0}},    {Direction::Right, {1, 0}},     {Direction::UpLeft, {-1, -1}},
    {Direction::UpRight, {1, -1}}, {Direction::DownLeft, {-1, 1}}, {Direction::DownRight, {1, 1}}};

BallController::BallController(SDL_FRect rect, bool showcase) : Node(rect) {
  isShowcase = showcase;

  int rows = isShowcase ? Game::Config::Grid::ROWS_SHOWCASE : Game::Config::Grid::ROWS;
  int cols = isShowcase ? Game::Config::Grid::COLUMNS_SHOWCASE : Game::Config::Grid::COLUMNS;

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      AddBallAt(GetRandomColor(), {c, r});
    }
  }

  if (isShowcase) return;

  EventManager::Instance().Subscribe("ballHit", this, [this](std::any data) { OnBallHitCallback(data); });
}

vec2 BallController::GridToWorld(vec2i gridPosition) {
  // Offset every other row to create staggered grid effect
  bool isEven = (gridPosition.y + scrolls) % 2 == 0;
  vec2 worldPosition;

  float xOffset = isEven ? 0.0f : 0.5f;
  worldPosition = {((float)gridPosition.x + xOffset) * Game::Config::Grid::SPACING_X + rect.x,
                   (float)gridPosition.y * Game::Config::Grid::SPACING_Y + rect.y};

  return worldPosition;
}

vec2i BallController::WorldToGrid(vec2 worldPosition) {
  vec2i gridPosition;
  gridPosition.y = (int)std::round((worldPosition.y - rect.y) / Game::Config::Grid::SPACING_Y);

  bool isEven = (gridPosition.y + scrolls) % 2 == 0;
  float xAdjustment = isEven ? 0.0f : 0.5f;
  gridPosition.x = (int)std::round(((worldPosition.x - rect.x) / Game::Config::Grid::SPACING_X) - xAdjustment);

  return gridPosition;
}

void BallController::OnBallHitCallback(std::any data) {
  auto payload = GameStateManager::Instance().GetPayload();
  CollisionData collisionData = std::any_cast<CollisionData>(data);
  Bullet *bullet = (Bullet *)collisionData.first;
  Ball *ball = (Ball *)collisionData.second;

  shots += 1;

  // Handle bullet missing all balls and leaving the play area
  if (!ball) {
    if (shots >= payload->Get<int>("difficulty.shots_to_scroll")) CreateNewLine();
    return;
  }

  Console::Log("Ball hit!");
  AudioManager::Instance().PlaySound("snd_bubble");
  bullet->Destroy();

  // Snap the bullet to the nearest valid empty grid cell
  Ball *newBall = AddBallAt(bullet->color, GetBestPlacementPosition(bullet, ball));

  if (shots >= payload->Get<int>("difficulty.shots_to_scroll")) CreateNewLine();

  // Recursive search for matching colors
  std::vector<std::shared_ptr<Ball>> result = {};
  GetSameColorBalls(newBall, result);
  int totalBalls = result.size();
  if (result.size() >= Game::Config::Rules::NECESSARY_BALLS_TO_DESTROY) {
    // Step 1: Remove matched colors
    for (const auto b : result) {
      balls.erase(b->GetGridPosition());
      b->Animate(Ball::Animation::Pop);
    }

    // Step 2: Find "hanging" islands
    GetIsolatedBalls(result);
    totalBalls += (result.size() * Game::Config::Rules::ISLAND_MULTIPLIER);

    for (const auto b : result) {
      balls.erase(b->GetGridPosition());
      b->Animate(Ball::Animation::Fall);
    }

    Console::Log("Total balls destroyed: " + std::to_string(totalBalls));
    EventManager::Instance().Emit("ballsDestroyed", totalBalls);
  }

  difficultyController->EvaluateDifficulty();
}

vec2i BallController::GetBestPlacementPosition(Bullet *bullet, Ball *ball) {
  vec2 bulletPos = bullet->GetCenter();
  std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(ball->GetGridPosition());

  float bestDistance = FLT_MAX;
  vec2i bestPosition = ball->GetGridPosition();
  vec2 halfCellOffset = {Game::Config::Grid::SPACING_X / 2.0f, Game::Config::Grid::SPACING_Y / 2.0f};

  // Find the closest empty neighbor slot to the collision point
  for (const auto &[gridPos, neighbour] : neighbors) {
    if (neighbour && !neighbour->IsPendingDestruction()) continue;

    vec2 cellCenter = GridToWorld(gridPos) + halfCellOffset;
    float distance = bulletPos.distance_sq(cellCenter);
    if (distance < bestDistance) {
      bestDistance = distance;
      bestPosition = gridPos;
    }
  }
  return bestPosition;
}

/**
 * Finds balls that are no longer connected to the top row (the "roof").
 * Uses a Breadth-First Search (BFS) starting from all balls in row 0.
 */
void BallController::GetIsolatedBalls(std::vector<std::shared_ptr<Ball>> &result) {
  std::unordered_set<std::shared_ptr<Ball>> connectedToRoof;
  std::vector<std::shared_ptr<Ball>> queue;

  // 1. Seed the BFS with all balls currently attached to the top row
  for (int c = 0; c < Game::Config::Grid::COLUMNS; c++) {
    auto ball = GetBallAt({c, 0});
    if (ball && !ball->IsPendingDestruction()) {
      queue.push_back(ball);
      connectedToRoof.insert(ball);
    }
  }

  // 2. Traverse all reachable balls from the roof
  int head = 0;
  while (head < queue.size()) {
    auto current = queue[head++];
    vec2i pos = current->GetGridPosition();

    std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(pos);
    for (const auto &[gridPos, neighbour] : neighbors) {
      if (!neighbour || neighbour->IsPendingDestruction()) continue;

      auto neighborShared = GetBallAt(neighbour->GetGridPosition());
      if (neighborShared && connectedToRoof.find(neighborShared) == connectedToRoof.end()) {
        connectedToRoof.insert(neighborShared);
        queue.push_back(neighborShared);
      }
    }
  }

  // 3. Any ball NOT in the 'connectedToRoof' set is floating and should be destroyed
  result.clear();
  for (const auto &[pos, ball] : balls) {
    if (connectedToRoof.find(ball) == connectedToRoof.end()) { result.push_back(ball); }
  }
}

void BallController::CreateNewLine() {
  shots = 0;
  Console::Log("Creating new line!");
  std::unordered_map<vec2i, std::shared_ptr<Ball>> updatedBalls;
  scrolls += 1;

  GameStateManager::Instance().GetPayload()->Set<int>("totalScrolls", scrolls);

  // Shift all existing balls down one row
  for (const auto &[pos, ball] : balls) {
    vec2i nextPos = {pos.x, pos.y + 1};
    ball->SetGridPosition(nextPos);
    ball->SetPosition(GridToWorld(nextPos));
    updatedBalls[nextPos] = ball;
    ball->collisionBox->UpdateRect(ball->rect);
  }
  balls = std::move(updatedBalls);

  // Populate the new top row
  for (int c = 0; c < Game::Config::Grid::COLUMNS; c++) {
    AddBallAt(GetRandomColor(), {c, 0});
  }
}

Ball *BallController::AddBallAt(Color color, vec2i position) {
  if (GetBallAt(position) != nullptr) return nullptr;

  auto ball = std::make_shared<Ball>(position);
  ball->SetPosition(GridToWorld(position));
  ball->color = color;
  ball->SetModulate(GetModulateFromEnum(color));

  balls[position] = ball;
  GameScene::Instance().AddNode(ball);

  if (!isShowcase) {
    SDL_FRect collisionRect = ball->rect;
    collisionRect.w *= Game::Config::Rules::COLLIDER_SCALE;
    collisionRect.h *= Game::Config::Rules::COLLIDER_SCALE;
    ball->AddCollisionBox(CollisionLayers::LAYER_1, CollisionLayers::LAYER_2 | CollisionLayers::LAYER_3, collisionRect);
  }
  return ball.get();
}

std::shared_ptr<Ball> BallController::GetBallAt(vec2i position) {
  auto it = balls.find(position);
  return (it != balls.end()) ? it->second : nullptr;
}

void BallController::GetSameColorBalls(Ball *ball, std::vector<std::shared_ptr<Ball>> &result) {
  if (!ball || ball->IsPendingDestruction()) return;

  // Prevent infinite recursion/duplicate entries
  for (const auto &existing : result) {
    if (existing.get() == ball) return;
  }

  vec2i pos = ball->GetGridPosition();
  result.push_back(GetBallAt(pos));

  // Recursive search through neighbors of the same color
  std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(pos);
  for (const auto &[gridPos, neighbour] : neighbors) {
    if (neighbour && !neighbour->IsPendingDestruction() && neighbour->color == ball->color) {
      GetSameColorBalls(neighbour, result);
    }
  }
}

std::unordered_map<vec2i, Ball *> BallController::GetNeighbors(vec2i position) {
  int row = static_cast<int>(position.y);
  bool isEven = ((row + scrolls) % 2 == 0);

  // In a staggered grid, "Vertical" neighbors' X-indices change based on row parity
  int offset = (isEven) ? 0 : 1;

  std::unordered_map<vec2i, Ball *> result;

  for (const auto &[dir, vec] : directionVectors) {
    int newX = position.x + vec.x;
    int newY = position.y + vec.y;

    // Apply staggering logic for Up/Down directions
    if (vec.y != 0) { newX = position.x + (vec.x < 0 ? offset - 1 : offset); }

    vec2i newPos = {newX, newY};
    auto surroundingBall = GetBallAt(newPos);

    result[newPos] = (surroundingBall && !surroundingBall->IsPendingDestruction()) ? surroundingBall.get() : nullptr;
  }

  return result;
}
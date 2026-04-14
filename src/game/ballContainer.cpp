#include "ballContainer.h"
#include "core/managers/audioManager.h"
#include "core/managers/collisionManager.h"
#include "core/managers/eventManager.h"
#include "core/managers/gameStateManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/utils/console.h"
#include "game/bubbleGameConfig.h"
#include "game/bullet.h"
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

std::unordered_map<Direction, vec2i> directionVectors = {
    {Direction::Left, {-1, 0}},    {Direction::Right, {1, 0}},     {Direction::UpLeft, {-1, -1}},
    {Direction::UpRight, {1, -1}}, {Direction::DownLeft, {-1, 1}}, {Direction::DownRight, {1, 1}}};

BallContainer::BallContainer(SDL_FRect rect, bool showcase) : Node(rect) {

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

vec2 BallContainer::GridToWorld(vec2i gridPosition) {
  bool isEven = (gridPosition.y + scrolls) % 2 == 0;
  vec2 worldPosition;
  if (isEven) {
    worldPosition = {(float)gridPosition.x * Game::Config::Grid::SPACING_X + rect.x,
                     (float)gridPosition.y * Game::Config::Grid::SPACING_Y + rect.y};
  } else {
    worldPosition = {((float)gridPosition.x + 0.5f) * Game::Config::Grid::SPACING_X + rect.x,
                     (float)gridPosition.y * Game::Config::Grid::SPACING_Y + rect.y};
  }

  return worldPosition;
}

vec2i BallContainer::WorldToGrid(vec2 worldPosition) {
  vec2i gridPosition;
  gridPosition.y = (int)std::round((worldPosition.y - rect.y) / Game::Config::Grid::SPACING_Y);
  bool isEven = (gridPosition.y + scrolls) % 2 == 0;
  if (isEven) {
    gridPosition.x = (int)std::round((worldPosition.x - rect.x) / Game::Config::Grid::SPACING_X);
  } else {
    gridPosition.x = (int)std::round(((worldPosition.x - rect.x) / Game::Config::Grid::SPACING_X) - 0.5f);
  }
  return gridPosition;
}

void BallContainer::OnBallHitCallback(std::any data) {
  auto payload = GameStateManager::Instance().GetPayload();

  CollisionData collisionData = std::any_cast<CollisionData>(data);
  Bullet *bullet = (Bullet *)collisionData.first;
  Ball *ball = (Ball *)collisionData.second;
  if (!ball) {
    if (shots >= payload->Get<int>("difficulty.shots_to_scroll")) CreateNewLine();
    return;
  }

  shots += 1;

  Console::Log("Ball hit!");

  AudioManager::Instance().PlaySound("snd_bubble");
  bullet->Destroy();

  vec2 bulletPos = bullet->GetCenter();
  std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(ball->GetGridPosition());

  float bestDistance = FLT_MAX;
  vec2i bestPosition = ball->GetGridPosition();
  for (const auto &[gridPos, neighbour] : neighbors) {
    if (neighbour && !neighbour->IsPendingDestruction()) continue;

    int distance = bulletPos.distance_sq(GridToWorld(gridPos));
    if (distance < bestDistance) {
      bestDistance = distance;
      bestPosition = gridPos;
    }
  }

  Ball *newBall = AddBallAt(bullet->color, bestPosition);
  if (shots >= payload->Get<int>("difficulty.shots_to_scroll")) CreateNewLine();

  std::vector<std::shared_ptr<Ball>> result = {};
  GetSameColorBalls(newBall, result);

  int totalBalls = result.size();

  if (result.size() >= Game::Config::Rules::NECESSARY_BALLS_TO_DESTROY) {
    for (const auto b : result) {
      balls.erase(b->GetGridPosition());
      b->Animate(Ball::Animation::Pop);
    }

    GetIsolatedBalls(result);
    totalBalls += result.size();
    for (const auto b : result) {
      balls.erase(b->GetGridPosition());
      b->Animate(Ball::Animation::Fall);
    }

    Console::Log("Total balls destroyed: " + std::to_string(totalBalls));
    EventManager::Instance().Emit("ballsDestroyed", totalBalls);
  }

  difficultyController->EvaluateDifficulty();
}

void BallContainer::GetIsolatedBalls(std::vector<std::shared_ptr<Ball>> &result) {
  std::unordered_set<std::shared_ptr<Ball>> connectedToRoof;
  std::vector<std::shared_ptr<Ball>> queue;

  for (int c = 0; c < Game::Config::Grid::COLUMNS; c++) {
    auto ball = GetBallAt({c, 0});
    if (ball && !ball->IsPendingDestruction()) {
      queue.push_back(ball);
      connectedToRoof.insert(ball);
    }
  }

  int head = 0;
  while (head < queue.size()) {
    auto current = queue[head++];
    vec2i pos = current->GetGridPosition();

    std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(pos);
    for (const auto &[gridPos, neighbour] : neighbors) {
      if (!neighbour || neighbour->IsPendingDestruction()) continue;
      vec2i neighborPos = neighbour->GetGridPosition();
      auto neighbor = GetBallAt(neighborPos);
      if (neighbor && !neighbor->IsPendingDestruction()) {
        if (connectedToRoof.find(neighbor) == connectedToRoof.end()) {
          connectedToRoof.insert(neighbor);
          queue.push_back(neighbor);
        }
      }
    }
  }

  result.clear();
  for (const auto &[pos, ball] : balls) {
    if (connectedToRoof.find(ball) == connectedToRoof.end()) { result.push_back(ball); }
  }
}

void BallContainer::CreateNewLine() {
  shots = 0;
  Console::Log("Creating new line!");
  std::unordered_map<vec2i, std::shared_ptr<Ball>> updatedBalls;
  scrolls += 1;
  GameStateManager::Instance().GetPayload()->Set<int>("totalScrolls", scrolls);
  for (const auto &[pos, ball] : balls) {
    vec2i nextPos = {pos.x, pos.y + 1};
    ball->SetGridPosition(nextPos);
    ball->SetPosition(GridToWorld(nextPos));
    updatedBalls[nextPos] = ball;
    ball->collisionBox->UpdateRect(ball->rect);
  }
  balls = std::move(updatedBalls);
  for (int c = 0; c < Game::Config::Grid::COLUMNS; c++) {
    AddBallAt(GetRandomColor(), {c, 0});
  }
}

Ball *BallContainer::AddBallAt(Color color, vec2i position) {
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

std::shared_ptr<Ball> BallContainer::GetBallAt(vec2i position) {
  auto it = balls.find(position);
  return (it != balls.end()) ? it->second : nullptr;
}

void BallContainer::GetSameColorBalls(Ball *ball, std::vector<std::shared_ptr<Ball>> &result) {
  if (!ball || ball->IsPendingDestruction()) return;

  // Check if already exists
  for (const auto &existing : result) {
    if (existing.get() == ball) return;
  }

  vec2i pos = ball->GetGridPosition();
  auto currentBallPtr = GetBallAt(pos);
  result.push_back(currentBallPtr);

  std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(pos);
  // In each direction, check if there's a ball and add it to the list
  for (const auto &[gridPos, neighbour] : neighbors) {
    if (!neighbour || neighbour->IsPendingDestruction()) continue;
    if (neighbour->color == ball->color) { GetSameColorBalls(neighbour, result); }
  }
}

std::unordered_map<vec2i, Ball *> BallContainer::GetNeighbors(vec2i position) {
  int row = static_cast<int>(position.y);
  bool isEven = ((row + scrolls) % 2 == 0);
  int offset = (isEven) ? 0 : 1;

  std::unordered_map<vec2i, Ball *> result;

  for (const auto &[dir, vec] : directionVectors) {
    int newX = position.x + vec.x;
    int newY = position.y + vec.y;

    // Depending on if the row is even or uneven then the Up(Left/Right)
    // and Down(Left/Right) depend on that value
    if (vec.y != 0) { newX = position.x + (vec.x < 0 ? offset - 1 : offset); }

    vec2i newPos = {newX, newY};

    auto surroundingBall = GetBallAt(newPos);
    if (!surroundingBall || surroundingBall->IsPendingDestruction())
      result[newPos] = nullptr;
    else
      result[newPos] = surroundingBall.get();
  }

  return result;
}
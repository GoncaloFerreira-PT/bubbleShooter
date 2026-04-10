#include "ballContainer.h"
#include "core/managers/textureManager.h"
#include <iostream>
#include <memory>
#include <unordered_map>
#include "core/managers/eventManager.h"
#include "game/cannon.h"
#include "core/modifiers/collisionBox.h"

enum Direction
{
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
    {Direction::Left, {-1, 0}},
    {Direction::Right, {1, 0}},
    {Direction::UpLeft, {-1, -1}},
    {Direction::UpRight, {1, -1}},
    {Direction::DownLeft, {-1, 1}},
    {Direction::DownRight, {1, 1}}};

BallContainer::BallContainer(SDL_FRect rect) : Node(rect)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLUMNS; c++)
        {
            AddBallAt(GetRandomColor(), {c, r});
        }
    }

    EventManager::Instance().Subscribe("ballHit", [this](std::any data)
                                       { OnBallHitCallback(data); });
}

vec2 BallContainer::GridToWorld(vec2i gridPosition)
{
    bool isEven = (gridPosition.y + scrolls) % 2 == 0;
    vec2 worldPosition;
    if (isEven)
    {
        worldPosition = {(float)gridPosition.x * SPACING.x + rect.x, (float)gridPosition.y * SPACING.y + rect.y};
    }
    else
    {
        worldPosition = {((float)gridPosition.x + 0.5f) * SPACING.x + rect.x, (float)gridPosition.y * SPACING.y + rect.y};
    }

    return worldPosition;
}


vec2i BallContainer::WorldToGrid(vec2 worldPosition)
{
    vec2i gridPosition;
    gridPosition.y = (int)std::round((worldPosition.y - rect.y) / SPACING.y);
    bool isEven = (gridPosition.y + scrolls) % 2 == 0;
    if (isEven)
    {
        gridPosition.x = (int)std::round((worldPosition.x - rect.x) / SPACING.x);
    }
    else
    {
        gridPosition.x = (int)std::round(((worldPosition.x - rect.x) / SPACING.x) - 0.5f);
    }
    return gridPosition;
}


void BallContainer::OnBallHitCallback(std::any data)
{
    shots += 1;

    Console::Log("Ball hit!");

    CollisionData collisionData = std::any_cast<CollisionData>(data);
    Bullet *bullet = (Bullet *)collisionData.first;
    Ball *ball = (Ball *)collisionData.second;
    if (!ball)
    {
        if (shots >= SHOTS_TO_SCROLL)
            CreateNewLine();
        return;
    }

    AudioManager::Instance().PlaySound("snd_bubble");
    bullet->Destroy();

    vec2 bulletPos = bullet->GetCenter();
    std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(ball->GetGridPosition());

    float bestDistance = FLT_MAX;
    vec2i bestPosition = ball->GetGridPosition();
    for (const auto &[gridPos, neighbour] : neighbors)
    {
        if (neighbour && !neighbour->IsPendingDestruction())
            continue;

        int distance = bulletPos.distance_sq(GridToWorld(gridPos));
        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestPosition = gridPos;
        }
    }

    std::vector<std::shared_ptr<Ball>> result = {};
    Ball *newBall = AddBallAt(bullet->color, bestPosition);
    GetSameColorBalls(newBall, result);

    int totalBalls = result.size();

    if (result.size() >= NECESSARY_BALLS_TO_DESTROY)
    {
        for (auto b : result)
        {
            balls.erase(b->GetGridPosition());
            b->Animate(Ball::Animation::Pop);
        }

        GetIsolatedBalls(result);
        totalBalls += result.size();
        for (auto b : result)
        {
            balls.erase(b->GetGridPosition());
            b->Animate(Ball::Animation::Fall);
        }

        Console::Log("Total balls destroyed: " + std::to_string(totalBalls));
        EventManager::Instance().Emit("ballsDestroyed", totalBalls);
    }

    if (shots >= SHOTS_TO_SCROLL)
        CreateNewLine();
}


void BallContainer::GetIsolatedBalls(std::vector<std::shared_ptr<Ball>> &result)
{
    std::unordered_set<std::shared_ptr<Ball>> connectedToRoof;
    std::vector<std::shared_ptr<Ball>> queue;

    for (int c = 0; c < COLUMNS; c++)
    {
        auto ball = GetBallAt({c, 0});
        if (ball && !ball->IsPendingDestruction())
        {
            queue.push_back(ball);
            connectedToRoof.insert(ball);
        }
    }

    int head = 0;
    while (head < queue.size())
    {
        auto current = queue[head++];
        vec2i pos = current->GetGridPosition();

        std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(pos);
        for (const auto &[gridPos, neighbour] : neighbors)
        {
            if (!neighbour || neighbour->IsPendingDestruction())
                continue;
            vec2i neighborPos = neighbour->GetGridPosition();
            auto neighbor = GetBallAt(neighborPos);
            if (neighbor && !neighbor->IsPendingDestruction())
            {
                if (connectedToRoof.find(neighbor) == connectedToRoof.end())
                {
                    connectedToRoof.insert(neighbor);
                    queue.push_back(neighbor);
                }
            }
        }
    }

    result.clear();
    for (auto const &[pos, ball] : balls)
    {
        if (connectedToRoof.find(ball) == connectedToRoof.end())
        {
            result.push_back(ball);
        }
    }
}

void BallContainer::CreateNewLine()
{
    shots = 0;
    Console::Log("Creating new line!");
    std::unordered_map<vec2i, std::shared_ptr<Ball>> updatedBalls;
    scrolls += 1;
    for (auto const &[pos, ball] : balls)
    {
        vec2i nextPos = {pos.x, pos.y + 1};
        ball->SetGridPosition(nextPos);
        ball->SetPosition(GridToWorld(nextPos));
        ball->collisionBox->UpdateCollision(ball->rect);
        updatedBalls[nextPos] = ball;
    }
    balls = std::move(updatedBalls);
    for (int c = 0; c < COLUMNS; c++)
    {
        AddBallAt(GetRandomColor(), {c, 0});
    }
}

Ball *BallContainer::AddBallAt(Color color, vec2i position)
{
    if (GetBallAt(position) != nullptr)
        return nullptr;

    auto ball = std::make_shared<Ball>(position);

    ball->SetPosition(GridToWorld(position));
    ball->color = color;
    ball->SetModulate(GetModulateFromEnum(color));

    SDL_FRect collisionRect = ball->rect;
    collisionRect.w *= 0.7f;
    collisionRect.h *= 0.7f;
    ball->AddCollisionBox(new CollisionBox(collisionRect));
    ball->collisionBox->UpdateCollision(ball->rect);
    balls[position] = ball;
    Scene::Instance().AddNode(ball);
    return ball.get();
}

std::shared_ptr<Ball> BallContainer::GetBallAt(vec2i position)
{
    auto it = balls.find(position);
    return (it != balls.end()) ? it->second : nullptr;
}

void BallContainer::GetSameColorBalls(Ball *ball, std::vector<std::shared_ptr<Ball>> &result)
{
    if (!ball || ball->IsPendingDestruction())
        return;

    // Check if already exists
    for (const auto &existing : result)
    {
        if (existing.get() == ball)
            return;
    }

    vec2i pos = ball->GetGridPosition();
    auto currentBallPtr = GetBallAt(pos);
    result.push_back(currentBallPtr);

    std::unordered_map<vec2i, Ball *> neighbors = GetNeighbors(pos);
    // In each direction, check if there's a ball and add it to the list
    for (const auto &[gridPos, neighbour] : neighbors)
    {
        if (!neighbour || neighbour->IsPendingDestruction())
            continue;
        if (neighbour->color == ball->color)
        {
            GetSameColorBalls(neighbour, result);
        }
    }
}

std::unordered_map<vec2i, Ball *> BallContainer::GetNeighbors(vec2i position)
{
    int row = static_cast<int>(position.y);
    bool isEven = ((row + scrolls) % 2 == 0);
    int offset = (isEven) ? 0 : 1;

    std::unordered_map<vec2i, Ball *> result;

    for (const auto &[dir, vec] : directionVectors)
    {
        int newX = position.x + vec.x;
        int newY = position.y + vec.y;

        // Depending on if the row is even or uneven then the Up(Left/Right)
        // and Down(Left/Right) depend on that value
        if (vec.y != 0)
        {
            newX = position.x + (vec.x < 0 ? offset - 1 : offset);
        }

        vec2i newPos = {newX, newY};

        auto surroundingBall = GetBallAt(newPos);
        if (!surroundingBall || surroundingBall->IsPendingDestruction())
            result[newPos] = nullptr;
        else
            result[newPos] = surroundingBall.get();
    }

    return result;
}
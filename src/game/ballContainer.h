#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include "ball.h"
#include "core/node/node.h"
#include "core/scene.h"
#include <any>
#include <unordered_map>
#include <unordered_set>
#include "core/managers/audioManager.h"


class BallContainer: public Node
{
private:
        std::unordered_map<vec2i,std::shared_ptr<Ball>> balls;
        const int COLUMNS = 10;
        const int ROWS = 5;
        const vec2i SPACING = {36, 36};
        const int NECESSARY_BALLS_TO_DESTROY = 3;
        const float DELAY_PER_BALL_DESTROYED = 0.1f;
        const int SHOTS_TO_SCROLL = 5;
        int shots = 0;
        int scrolls = 0;

        //Pattern
        //InitialLines = 10

    public:
        BallContainer(SDL_FRect rect);
        Ball* AddBallAt(Color color, vec2i position);
        std::shared_ptr<Ball> GetBallAt(vec2i position);
        void OnBallHitCallback(std::any data);
       
        void CreateNewLine();
        void GetSameColorBalls(Ball *ball, std::vector<std::shared_ptr<Ball>> &result);
        void GetIsolatedBalls(std::vector<std::shared_ptr<Ball>> &result);
        std::unordered_map<vec2i, Ball*> GetNeighbors(vec2i position);
        vec2 GridToWorld(vec2i gridPosition);
        vec2i WorldToGrid(vec2 worldPosition);
};



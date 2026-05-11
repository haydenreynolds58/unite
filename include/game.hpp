#pragma once

#include "input_manager.hpp"
#include "player.hpp"
#include "arrow.hpp"

#include <raylib.h>

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    void HandleInput(float dt);
    void Update(float dt);
    void Draw() const;

    Vector2 CalculateAimDirection() const;

    InputManager input;
    Player player;
    Arrow arrow{{640.0f, 360.0f}};

    Vector2 aimDirection{1.0f, 0.0f};

    int screenWidth = 1280;
    int screenHeight = 720;
};
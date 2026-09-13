#include "game.hpp"

#include <raylib.h>
#include <cmath>

static Vector2 NormalizeOrDefault(Vector2 v, Vector2 fallback) {
    float length = std::sqrt(v.x * v.x + v.y * v.y);

    if (length == 0.0f) {
        return fallback;
    }

    return {
        v.x / length,
        v.y / length
    };
}

Game::Game() {
    InitWindow(screenWidth, screenHeight, "Archer Game");
    SetTargetFPS(60);
}

Game::~Game() {
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        HandleInput(dt);
        Update(dt);
        Draw();
    }
}

void Game::HandleInput(float dt) {
    input.Update();

    player.SetMoveInput(input.GetMoveInput());
    player.SetAimTarget(input.GetMousePosition());
}

void Game::Update(float dt) {
    player.Update(dt);

    aimDirection = CalculateAimDirection();

    arrow.Update(
        input.IsShootHeld(),
        aimDirection,
        player.GetPosition(),
        dt
    );
}

void Game::Draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    player.Draw();
    arrow.Draw(aimDirection);

    EndDrawing();
}

Vector2 Game::CalculateAimDirection() const {
    Vector2 playerPosition = player.GetPosition();
    Vector2 m_mousePosition = input.GetMousePosition();

    Vector2 direction{
        m_mousePosition.x - playerPosition.x,
        m_mousePosition.y - playerPosition.y
    };

    return NormalizeOrDefault(direction, {1.0f, 0.0f});
}

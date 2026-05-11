#pragma once
#include <raylib.h>

class InputManager {
public:
    void Update();

    Vector2 GetMoveInput() const;
    Vector2 GetMousePosition() const;

    bool IsShootHeld() const;

private:
    Vector2 moveInput{0.0f, 0.0f};
    Vector2 mousePosition{0.0f, 0.0f};

    bool shootHeld = false;
};

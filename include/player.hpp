#pragma once
#include <raylib.h>

class Player {
public:
    void SetMoveInput(Vector2 input);
    void SetAimTarget(Vector2 target);

    Vector2 GetPosition() const;

    void Update(float dt);
    void Draw() const;

private:
    Vector2 position{640.0f, 360.0f};
    Vector2 moveInput{0.0f, 0.0f};
    Vector2 aimTarget{640.0f, 360.0f};

    float speed = 250.0f;
};

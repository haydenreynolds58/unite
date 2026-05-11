#pragma once

#include <raylib.h>

class Arrow {
public:
    Arrow(Vector2 startPosition);

    void Update(
        bool mouseHeld,
        Vector2 direction,
        Vector2 playerPosition,
        float dt
    );

    void Draw(Vector2 aimDirection) const;

private:
    Vector2 position{0.0f, 0.0f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 decelerationDirection{0.0f, 0.0f};

    // Used to remember the arrow's last direction after it stops
    Vector2 oldVelocity{1.0f, 0.0f};

    bool equipped = true;
    bool charging = false;
    bool flying = false;

    float speed = 500.0f;
    float velocityCharge = 1.0f;
    float maxVelocityCharge = 1.775f;
    float chargeRate = 0.75f;
    float decelerationAmount = 0.05f;
};
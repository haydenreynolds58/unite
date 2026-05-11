#include "arrow.hpp"
#include "vector_math.hpp"

#include <algorithm>
#include <cmath>

Arrow::Arrow(Vector2 startPosition) {
    position = startPosition;
}

void Arrow::Update(
    bool mouseHeld,
    Vector2 direction,
    Vector2 playerPosition,
    float dt
) {
    if (equipped) {
        position = playerPosition;

        if (!charging) {
            velocityCharge = 1.0f;
        }
    }

    if (mouseHeld && equipped) {
        charging = true;

        velocityCharge = std::min(
            velocityCharge + chargeRate * dt,
            maxVelocityCharge
        );

    }
    else if (!mouseHeld && charging) {
        charging = false;
        flying = true;
        equipped = false;

        velocity = Vec::Scale(direction, speed * velocityCharge);
        decelerationDirection = direction;
        oldVelocity = velocity;
    }
    else if (flying) {
        if (position.x >= GetScreenWidth() || position.x <= 0.0f) {
            velocity.x = -velocity.x;
            decelerationDirection.x = -decelerationDirection.x;
        }

        if (position.y >= GetScreenHeight() || position.y <= 0.0f) {
            velocity.y = -velocity.y;
            decelerationDirection.y = -decelerationDirection.y;
        }

        velocity = Vec::ApproachZero(
            velocity,
            Vec::Scale(decelerationDirection, speed * decelerationAmount)
        );

        position = Vec::Add(position, Vec::Scale(velocity, dt));

        if (velocity.x != 0.0f || velocity.y != 0.0f) {
            oldVelocity = velocity;
        }

        if (velocity.x == 0.0f && velocity.y == 0.0f) {
            flying = false;
            equipped = true;
        }
    }
}

void Arrow::Draw(Vector2 aimDirection) const {
    Rectangle arrowRect{
        position.x,
        position.y,
        10.0f,
        22.0f
    };

    Vector2 origin{
        arrowRect.width / 2.0f,
        arrowRect.height / 2.0f
    };

    if (equipped) {
        DrawRectanglePro(
            arrowRect,
            origin,
            -90.0f + atan2f(aimDirection.y, aimDirection.x) * RAD2DEG,
            RED
        );
    }
    else if (flying) {
        DrawRectanglePro(
            arrowRect,
            origin,
            -90.0f + atan2f(velocity.y, velocity.x) * RAD2DEG,
            RED
        );
    }
    else {
        DrawRectanglePro(
            arrowRect,
            origin,
            -90.0f + atan2f(oldVelocity.y, oldVelocity.x) * RAD2DEG,
            RED
        );
    }
}

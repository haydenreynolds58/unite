#include "input_manager.hpp"
#include <cmath>

static Vector2 NormalizeOrZero(Vector2 v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y);

    if (length == 0.0f) {
        return {0.0f, 0.0f};
    }

    return {
        v.x / length,
        v.y / length
    };
}

void InputManager:: Update() {
    moveInput = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W)) moveInput.y -= 1.0f;
    if (IsKeyDown(KEY_S)) moveInput.y += 1.0f;
    if (IsKeyDown(KEY_A)) moveInput.x -= 1.0f;
    if (IsKeyDown(KEY_D)) moveInput.x += 1.0f;

    moveInput = NormalizeOrZero(moveInput);

    mousePosition = ::GetMousePosition();

    shootHeld = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

Vector2 InputManager::GetMoveInput() const {
    return moveInput;
}

Vector2 InputManager::GetMousePosition() const {
    return mousePosition;
}

bool InputManager::IsShootHeld() const {
    return shootHeld;
}

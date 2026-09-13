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
    m_moveInput = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W)) m_moveInput.y -= 1.0f;
    if (IsKeyDown(KEY_S)) m_moveInput.y += 1.0f;
    if (IsKeyDown(KEY_A)) m_moveInput.x -= 1.0f;
    if (IsKeyDown(KEY_D)) m_moveInput.x += 1.0f;

    m_moveInput = NormalizeOrZero(m_moveInput);

    m_mousePosition = ::GetMousePosition();

    m_shootHeld = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

Vector2 InputManager::GetMoveInput() const {
    return m_moveInput;
}

Vector2 InputManager::GetMousePosition() const {
    return m_mousePosition;
}

bool InputManager::IsShootHeld() const {
    return m_shootHeld;
}

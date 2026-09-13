#include "player.hpp"

void Player::SetMoveInput(Vector2 input) {
    m_moveInput = input;
}

void Player::SetAimTarget(Vector2 target) {
    m_aimTarget = target;
}

Vector2 Player::GetPosition() const {
    return m_position;
}

void Player::Update(float dt) {
    m_position.x += m_moveInput.x * speed * dt;
    m_position.y += m_moveInput.y * speed * dt;
}

void Player::Draw() const {
    DrawCircleV(m_position, 20.0f, BLUE);

    DrawLineV(m_position, m_aimTarget, RED);
}

#include "player.hpp"

void Player::SetMoveInput(Vector2 input) {
    moveInput = input;
}

void Player::SetAimTarget(Vector2 target) {
    aimTarget = target;
}

Vector2 Player::GetPosition() const {
    return position;
}

void Player::Update(float dt) {
    position.x += moveInput.x * speed * dt;
    position.y += moveInput.y * speed * dt;
}

void Player::Draw() const {
    DrawCircleV(position, 20.0f, BLUE);

    DrawLineV(position, aimTarget, RED);
}

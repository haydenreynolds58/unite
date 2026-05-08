#include "player.hpp"

Vector2 Player::position = {500, 700};

Player::Player(){
    speed = 250.0f;
};

void Player::Update(float x_inc, float y_inc, float dt){
    position.x += x_inc * speed * dt;
    position.y += y_inc * speed * dt;
};
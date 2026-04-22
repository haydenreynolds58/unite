#include "player.hpp"
#include <algorithm>

Vector2 Player::position = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
Player::Player(){
    speed = 250.0f;
};

void Player::Update(float x_inc, float y_inc, float dt){
    position.x += x_inc * speed * dt;
    position.y += y_inc * speed * dt;
};

Arrow::Arrow(Vector2 position){
    this -> position = position;
}

void Arrow::Update(bool mouse_clicked, Vector2 dir, float dt){
    if(equipped){
        position = Player::position;
    };

    if(mouse_clicked && equipped){
        charging = true;
        vel_charge = std::min(vel_charge + 0.01f, 2.0f);
    } 

    // transition from charging to flying
    else if(!mouse_clicked && charging){
        charging = false;
        flying = true;
        equipped = false;

        velocity.x = speed * dir.x * vel_charge;
        velocity.y = speed * dir.y * vel_charge;
    }

    else if(flying){
        velocity.x = std::max(velocity.x - 2.0f, 0.0f);
        velocity.y = std::max(velocity.y - 2.0f, 0.0f);
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        if(!velocity.x && !velocity.y){
            flying = false;
        };
    };
}

void Arrow::Draw(){
    DrawRectangle(position.x, position.y, 10, 59, RED);
};
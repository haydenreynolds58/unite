#include "player.hpp"
#include "vector_math.hpp"
#include <algorithm>
#include <math.h>
#include <cstdio>

Vector2 Player::position = {500, 700};
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
        if(!charging){
            vel_charge = 1; 
        };
    };

    if(mouse_clicked && equipped){
        charging = true;
        vel_charge = std::min(vel_charge + 0.05f, 1.775f);
        
        printf("vel charge: %f\n", vel_charge);
    } 

    // transition from charging to flying
    else if(!mouse_clicked && charging){
        charging = false;
        flying = true;
        equipped = false;
        printf("RELEASES: vel charge: %f\n", vel_charge);

        velocity = Vec::Scale(dir, speed * vel_charge);
        deceleration = dir;
    }

    else if(flying){
        if(position.x >= GetScreenWidth() || position.x <= 0){
            velocity.x = -velocity.x;
            deceleration.x = -deceleration.x;
        }
        if(position.y >= GetScreenHeight() || position.y <= 0){
            velocity.y = -velocity.y;
            deceleration.y = -deceleration.y;
        }

        velocity = Vec::ApproachZero(velocity, Vec::Scale(deceleration, speed * 0.05f));
        position = Vec::Add(position, velocity * dt);


        if(!velocity.x && !velocity.y){
            flying = false;
            equipped = true;
        };
    };
}

void Arrow::Draw(Vector2 dir){
    Rectangle arrow{position.x, position.y, 10.0f, 22.0f};
    Vector2 old_vel; // store velocity for rotation before decelerated to zero
    Vector2 origin = {0.0f, 0.0f};
    if(equipped){
        DrawRectanglePro(arrow, origin, -90 + (atan2f(dir.y, dir.x) * RAD2DEG), RED);
    }

    if(flying){
        DrawRectanglePro(arrow, origin, -90 + (atan2f(velocity.y, velocity.x) * RAD2DEG), RED);

        Vector2 old_vel = velocity;
    }

    else if (!flying && !equipped){
        DrawRectanglePro(arrow, origin, -90 + (atan2f(old_vel.y, old_vel.x) * RAD2DEG), RED);
    };
}
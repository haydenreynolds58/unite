#include "player.hpp"
#include <algorithm>

Vector2 Player::position = {500, 500};
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

        deceleration.x = dir.x;
        deceleration.y = dir.y;
    }

    else if(flying){
        velocity.x = std::max(velocity.x - 2.0f * deceleration.x , 0.0f);
        velocity.y = std::max(velocity.y - 2.0f * deceleration.y, 0.0f);
        position.x += velocity.x * dt;
        position.y += velocity.y * dt ;
        if(!velocity.x && !velocity.y){
            flying = false;
        };
    };
}

void Arrow::Draw(){
    DrawRectangle(position.x, position.y, 10, 59, RED);
};


// Vector2 Player::position = {500, 700};
// Player::Player(){
//     speed = 250.0f;
// };

// void Player::Update(float x_inc, float y_inc, float dt){
//     position.x += x_inc * speed * dt;
//     position.y += y_inc * speed * dt;
// };

// Arrow::Arrow(Vector2 position){
//     this -> position = position;
// }

// void Arrow::Update(bool mouse_clicked, Vector2 dir, float dt){
//     if(equipped){
//         position = Player::position;
//         vel_charge = 1; 
//     };

//     if(mouse_clicked && equipped){
//         charging = true;
//         vel_charge = std::min(vel_charge + 0.01f, 2.0f);
//     } 

//     // transition from charging to flying
//     else if(!mouse_clicked && charging){
//         charging = false;
//         flying = true;
//         equipped = false;

//         velocity.x = speed * dir.x * vel_charge;
//         velocity.y = speed * dir.y * vel_charge;

//         deceleration.x = dir.x;
//         deceleration.y = dir.y;
//     }

//     else if(flying){
//         velocity.x = std::max(velocity.x - 2.0f * deceleration.x, 0.0f);
//         velocity.y = std::max(velocity.y - 2.0f * deceleration.y, 0.0f);
//         position.x += velocity.x * dt;
//         position.y += velocity.y * dt ;
//         if(!velocity.x && !velocity.y){
//             flying = false;
//         };
//     };
// }

// void Arrow::Draw(Vector2 dir){
//     Rectangle arrow{position.x, position.y, 10.0f, 22.0f};
//     Vector2 origin = {0.0f, 0.0f};
//     if(equipped){
//         DrawRectanglePro(arrow, origin, -90 + (atan2f(dir.y, dir.x) * RAD2DEG), RED);
//     }

//     if(flying){
//         DrawRectangle(position.x, position.y, 10, 59, RED);
//     }
// };
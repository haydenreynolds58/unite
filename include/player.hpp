#pragma once
#include <raylib.h>
class Player {
    public:
        Player();
        void Update(float x_delta, float y_delta, float dt);
        static Vector2 position;
    private:
        float speed;

};

class Arrow {
    public:
        Arrow(Vector2 position);
        void Update(bool mouse_clicked, Vector2 dir, float dt);
        void Draw();

    private:
        bool charging = false;
        bool flying = false;
        bool equipped = true;
        float speed = 250;
        float vel_charge = 1;
        Vector2 position;
        Vector2 velocity;

        // if mouse button held && not fired

            // enter charge state and animation 
            
            // increase vel_charge to some cap 

        // if mouse button not held && in charge state

            // set charge to false
            // call change position by velocity vector (direction * dt * vel_charge * speed)
        

};
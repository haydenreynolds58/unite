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
        void Draw(Vector2 dir);

    private:
        bool charging = false;
        bool flying = false;
        bool equipped = true;
        float speed = 800;
        float vel_charge = 1;
        Vector2 position;
        Vector2 velocity;
        Vector2 deceleration;
};
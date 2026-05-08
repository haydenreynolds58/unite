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
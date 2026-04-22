#include <raylib.h>   // Main raylib header: windowing, drawing, input, audio, etc.
#include <cmath>      // For sqrtf()
#include "../include/player.hpp"

int main() {

    // Create the window and give it a title
    InitWindow(1280, 720, "Archer MVP");

    // Tell raylib we want the game to try to run at 60 frames per second
    SetTargetFPS(60);

    // Initialise player with hardcode position and speed
    Player player;
    Arrow arrow(player.position);

    // Main game loop:
    // Runs every frame until the player closes the window or presses Escape
    while (!WindowShouldClose()) {
        // Time since last frame, using dt makes movement frame-rate independent.
        float dt = GetFrameTime();

        // Reset movement direction for this frame
        Vector2 move{0.0f, 0.0f};

        // Read keyboard input and build a movement vector
        if (IsKeyDown(KEY_W)) move.y -= 1.0f;   // up
        if (IsKeyDown(KEY_S)) move.y += 1.0f;   // down
        if (IsKeyDown(KEY_A)) move.x -= 1.0f;   // left
        if (IsKeyDown(KEY_D)) move.x += 1.0f;   // right

        // If moving diagonally, normalize the vector so diagonal movement
        // is not faster than horizontal/vertical movement
        if (move.x != 0.0f && move.y != 0.0f) {
            float len = sqrtf(move.x * move.x + move.y * move.y);
            move.x /= len;
            move.y /= len;
        }

        Vector2 mouse = GetMousePosition();
        Vector2 dir = {mouse.x - player.position.x, mouse.y - player.position.y};

        if (dir.x != 0.0f && dir.y != 0.0f) {
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            dir.x /= len;
            dir.y /= len;
        }

        // Update player position based on movement direction, speed, and frame time
        player.Update(move.x, move.y, dt);
        arrow.Update(IsMouseButtonDown(MOUSE_BUTTON_LEFT), dir, dt);

        // Start drawing this frame
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the player as a white circle
        DrawCircleV(player.position, 20.0f, RAYWHITE);
        DrawLineV(player.position, mouse, RED);
        arrow.Draw();

        // Draw some simple instructions
        DrawText("WASD to move", 20, 20, 20, RAYWHITE);

        // Finish drawing and present the frame to the screen
        EndDrawing();
    }

    // Clean up raylib resources and close the window
    CloseWindow();
    return 0;
}
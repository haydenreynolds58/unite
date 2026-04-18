#include <raylib.h>   // Main raylib header: windowing, drawing, input, audio, etc.
#include <cmath>      // For sqrtf()

int main() {
    // Set the size of the game window
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Create the window and give it a title
    InitWindow(screenWidth, screenHeight, "Archer MVP");

    // Tell raylib we want the game to try to run at 60 frames per second
    SetTargetFPS(60);

    // Store the player's position in 2D space, starting in the middle of the screen
    Vector2 player{screenWidth / 2.0f, screenHeight / 2.0f};

    // How fast the player moves, in pixels per second
    const float speed = 250.0f;

    // Main game loop:
    // Runs every frame until the player closes the window or presses Escape
    while (!WindowShouldClose()) {
        // Time since last frame.
        // Using dt makes movement frame-rate independent.
        float dt = GetFrameTime();

        // Movement direction for this frame
        Vector2 move{0.0f, 0.0f};

        // Read keyboard input and build a movement vector
        if (IsKeyDown(KEY_W)) move.y -= 1.0f;   // up
        if (IsKeyDown(KEY_S)) move.y += 1.0f;   // down
        if (IsKeyDown(KEY_A)) move.x -= 1.0f;   // left
        if (IsKeyDown(KEY_D)) move.x += 1.0f;   // right

        // If moving diagonally, normalize the vector so diagonal movement
        // is not faster than horizontal/vertical movement
        if (move.x != 0.0f || move.y != 0.0f) {
            float len = sqrtf(move.x * move.x + move.y * move.y);
            move.x /= len;
            move.y /= len;
        }

        // Update player position based on movement direction, speed, and frame time
        player.x += move.x * speed * dt;
        player.y += move.y * speed * dt;

        // Start drawing this frame
        BeginDrawing();

        // Clear the screen to black before drawing new objects
        ClearBackground(BLACK);

        // Draw the player as a white circle
        DrawCircleV(player, 20.0f, RAYWHITE);

        // Draw some simple instructions
        DrawText("WASD to move", 20, 20, 20, RAYWHITE);

        // Finish drawing and present the frame to the screen
        EndDrawing();
    }

    // Clean up raylib resources and close the window
    CloseWindow();
    return 0;
}
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "draw.h"

void init(TimeClusterState *state)
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "TimeCluster");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    state->camera = camera;
    // RenderTexture2D target = LoadRenderTexture(1920, 1080);
    // SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(60);
}

void draw(TimeClusterState *state)
{
    // Translate based on mouse right click
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/state->camera.zoom);

        state->camera.target = Vector2Add(state->camera.target, delta);
    }

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state->camera);
        
        // Set the offset to where the mouse is
        state->camera.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point 
        // under the cursor to the screen space point under the cursor at any zoom
        state->camera.target = mouseWorldPos;

        // Zoom increment
        const float zoomIncrement = 0.125f;

        state->camera.zoom += (wheel*zoomIncrement);
        if (state->camera.zoom < zoomIncrement) state->camera.zoom = zoomIncrement;
    }

    BeginDrawing();
    {
        ClearBackground(WHITE);

        BeginMode2D(state->camera);
        {
            // TODO draw everything and check if it has good performance
            // otherwise, try to scale/pan without BeginMode2D
            // lastly, try drawing texture
            DrawRectangleLines(0, 0, 300, 300, BLACK);

            DrawRectangleLines(500, 500, 300, 300, BLACK);

            DrawRectangleLines(500, 500, 300, 300, BLACK);

            DrawText("Implement point selection", 0, 0, 30, RED);
        }    
        EndMode2D();
    }
    
    EndDrawing();
}


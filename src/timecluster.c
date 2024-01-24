#include "raylib.h"
#include "rlgl.h"
#include "draw.h"

int main ()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "TimeCluster");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        draw(&camera);
    }

    CloseWindow();
    return 0;
}

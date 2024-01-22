#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void draw(Camera2D *camera)
{
    // Translate based on mouse right click
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera->zoom);

        camera->target = Vector2Add(camera->target, delta);
    }

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
        
        // Set the offset to where the mouse is
        camera->offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point 
        // under the cursor to the screen space point under the cursor at any zoom
        camera->target = mouseWorldPos;

        // Zoom increment
        const float zoomIncrement = 0.125f;

        camera->zoom += (wheel*zoomIncrement);
        if (camera->zoom < zoomIncrement) camera->zoom = zoomIncrement;
    }

    BeginDrawing();
    {
        ClearBackground(WHITE);

        BeginMode2D(*camera);
        {
            // Draw the 3d grid, rotated 90 degrees and centered around 0,0 
            // just so we have something in the XY plane
            rlPushMatrix();
            {
                rlTranslatef(0, 25*50, 0);
                rlRotatef(90, 1, 0, 0);
                DrawGrid(100, 50);
            }
            rlPopMatrix();

            // Draw a reference circle
            DrawCircle(100, 100, 50, BLUE);
        }    
        EndMode2D();
    }
    
    EndDrawing();
}

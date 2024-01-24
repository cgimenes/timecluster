#include <stddef.h>
#include <stdio.h>

#include <raylib.h>

#include <dlfcn.h>

#include "draw.h"

draw_t draw = NULL;

int main ()
{
    const char *libdraw_file_name = "libdraw.so";
    void *libdraw = dlopen(libdraw_file_name, RTLD_NOW);
    if (libdraw == NULL) {
        fprintf(stderr, "ERROR: could not load %s: %s", libdraw_file_name, dlerror());
        return 1;
    }

    draw = dlsym(libdraw, "draw");
    if (draw == NULL) {
        fprintf(stderr, "ERROR: could not find draw symbol in %s: %s", libdraw_file_name, dlerror());
        return 1;
    }

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

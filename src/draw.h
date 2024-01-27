#ifndef DRAW_H_
#define DRAW_H_

#include <raylib.h>

typedef struct TimeClusterState {
    Camera2D camera;
    // RenderTexture2D *target;
} TimeClusterState;

typedef void (*draw_t)(TimeClusterState *state);

typedef void (*init_t)(TimeClusterState *state);

#endif // DRAW_H_

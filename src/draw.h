#ifndef DRAW_H_
#define DRAW_H_

#include <raylib.h>

#define N 100

typedef struct TimeClusterState {
  // RenderTexture2D *target;
  Camera2D camera1;
  Camera2D camera2;
  float ts_data[N];
  Vector2 dr_data[N];
  int data_count;
  bool drawing_mode;
  Rectangle selection;
  bool selected_data[N];
} TimeClusterState;

typedef void (*draw_t)(TimeClusterState *state);

typedef void (*init_t)(TimeClusterState *state);

#endif // DRAW_H_

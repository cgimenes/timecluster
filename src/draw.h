#ifndef DRAW_H_
#define DRAW_H_

#include <raylib.h>

#define N 100

typedef float TimeseriesData[N];
typedef Vector2 DRData[N];

typedef struct TimeClusterState {
  // RenderTexture2D *target;
  Camera2D camera;
  TimeseriesData ts_data;
  DRData dr_data;
  int data_count;
  bool drawing_mode;
  Rectangle selection;
} TimeClusterState;

typedef void (*draw_t)(TimeClusterState *state);

typedef void (*init_t)(TimeClusterState *state);

#endif // DRAW_H_

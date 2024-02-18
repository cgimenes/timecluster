#ifndef RENDER_H_
#define RENDER_H_

#include <raylib.h>

#define MAX_FILEPATH_SIZE 2048

typedef struct TimeClusterState {
  Camera2D camera1;
  Camera2D camera2;
  int data_count;
  float *ts_data;
  Vector2 *dr_data;
  bool selection_mode;
  Rectangle selection;
  int *selected_data;
  char filePath[MAX_FILEPATH_SIZE];
} TimeClusterState;

typedef void (*render_t)(TimeClusterState *state);

typedef TimeClusterState (*init_t)();

#endif // RENDER_H_

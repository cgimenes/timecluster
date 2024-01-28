#include "draw.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#define RECT_SIZE 4.0f
#define MIDDLE_RECT_POINT 2.0f
#define X_SPACING 6
#define MOUSE_SCALE_MARK_SIZE 8

void init(TimeClusterState *state) {
  const int screenWidth = 800;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "TimeCluster");

  Camera2D camera = {0};
  camera.zoom = 1.0f;
  camera.target = (Vector2){-500, state->ts_data[0] - 500};

  state->camera = camera;
  state->drawing_mode = false;
  state->selection = (Rectangle){0};

  // RenderTexture2D target = LoadRenderTexture(1920, 1080);
  // SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

  SetTargetFPS(60);
}

void draw_rectangle_lines(float x, float y, float width, float height,
                          Color color) {
  Vector2 lines[5] = {
      {x, y}, {x + width, y}, {x + width, y + height}, {x, y + height}, {x, y},
  };
  DrawLineStrip(lines, 5, color);
}

void draw_rectangle_lines_ex(Rectangle rec, Color color) {
  draw_rectangle_lines(rec.x, rec.y, rec.width, rec.height, color);
}

void draw_timeseries(TimeClusterState *state) {
  // TODO draw everything and check if it has good performance
  // otherwise, try to scale/pan without BeginMode2D
  // lastly, try drawing texture
  for (int i = 0; i < state->data_count; i++) {
    float x = i * X_SPACING;
    float y = state->ts_data[i];
    Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};

    // TODO change to DrawLineStrip
    if (i > 0)
      DrawLine((i - 1) * X_SPACING + MIDDLE_RECT_POINT,
               state->ts_data[i - 1] + MIDDLE_RECT_POINT, x + MIDDLE_RECT_POINT,
               y + MIDDLE_RECT_POINT, BLACK);

    draw_rectangle_lines_ex(rec, BLACK);
  }
  for (int i = 0; i < state->data_count; i++) {
    if (! state->selected_data[i]) {
      continue;
    }

    float x = i * X_SPACING;
    float y = state->ts_data[i];

    Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};
    DrawRectangleRec(rec, YELLOW);
    draw_rectangle_lines_ex(rec, BLACK);
  }
}

void draw_hud(TimeClusterState *state) {
  // char str[80];
  // for (int i = 0; i < state->data_count; i++) {
  //   sprintf(str, "%d: %d", i, state->selected_data[i]);
  //   DrawText(str, 0, i * 22, 20, RED);
  // }
}

void handle_pan(TimeClusterState *state) {
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f / state->camera.zoom);

    state->camera.target = Vector2Add(state->camera.target, delta);
  }
}

void handle_zoom(TimeClusterState *state) {
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouseWorldPos =
        GetScreenToWorld2D(GetMousePosition(), state->camera);

    state->camera.offset = GetMousePosition();

    state->camera.target = mouseWorldPos;

    const float zoomIncrement = 0.125f;

    state->camera.zoom += (wheel * zoomIncrement);
    if (state->camera.zoom < zoomIncrement)
      state->camera.zoom = zoomIncrement;
  }
}

void handle_selection(TimeClusterState *state) {
  Vector2 mousePosition = GetMousePosition();
  Vector2 worldMousePosition = GetScreenToWorld2D(mousePosition, state->camera);

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !state->drawing_mode) {
    state->drawing_mode = true;
    state->selection.x = worldMousePosition.x;
    state->selection.y = worldMousePosition.y;
  }

  if (state->drawing_mode) {
    state->selection.width = (worldMousePosition.x - state->selection.x);
    state->selection.height = (worldMousePosition.y - state->selection.y);

    if (state->selection.width < MOUSE_SCALE_MARK_SIZE ||
        state->selection.height < MOUSE_SCALE_MARK_SIZE) {
      SetMousePosition(mousePosition.x + MOUSE_SCALE_MARK_SIZE,
                       mousePosition.y + MOUSE_SCALE_MARK_SIZE);
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      state->drawing_mode = false;

    for (int i = 0; i < state->data_count; i++) {
      float x = i * X_SPACING;
      float y = state->ts_data[i];
      Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};

      state->selected_data[i] = CheckCollisionRecs(state->selection, rec); 
    }
  }
}

void draw_selection(TimeClusterState *state) {
  if (state->drawing_mode) {
    DrawRectangleRec(state->selection, Fade(BLUE, 0.2f));

    draw_rectangle_lines_ex(state->selection, BLUE);
    // TODO draw a fixed sized triangle independent from camera.zoom
    DrawTriangle((Vector2){state->selection.x + state->selection.width -
                               MOUSE_SCALE_MARK_SIZE,
                           state->selection.y + state->selection.height},
                 (Vector2){state->selection.x + state->selection.width,
                           state->selection.y + state->selection.height},
                 (Vector2){state->selection.x + state->selection.width,
                           state->selection.y + state->selection.height -
                               MOUSE_SCALE_MARK_SIZE},
                 BLUE);
  }
}

void draw(TimeClusterState *state) {
  handle_pan(state);
  handle_zoom(state);
  handle_selection(state);

  BeginDrawing();
  {
    ClearBackground(WHITE);

    BeginMode2D(state->camera);
    {
      draw_selection(state);
      draw_timeseries(state);
    }
    EndMode2D();

    draw_hud(state);
  }
  EndDrawing();
}

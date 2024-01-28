#include "draw.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stddef.h>

#define RECT_SIZE 4.0f
#define MIDDLE_RECT_POINT 2.0f
#define MOUSE_SCALE_MARK_SIZE 8

void init(TimeClusterState *state) {
  const int screenWidth = 800;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "TimeCluster");

  Camera2D camera = {0};
  camera.zoom = 1.0f;

  state->camera1 = camera;
  state->camera1.target = (Vector2){-500, state->ts_data[0] - 500};

  state->camera2 = camera;
  state->camera2.target = (Vector2){-500, state->ts_data[0] - 500};

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

void draw_dr_data(TimeClusterState *state) {
  // TODO draw everything and check if it has good performance
  // otherwise, try to scale/pan without BeginMode2D
  // lastly, try drawing texture
  for (int i = 0; i < state->data_count; i++) {
    float x = i * (RECT_SIZE + 2);
    float y = state->ts_data[i];
    Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};

    // TODO change to DrawLineStrip
    if (i > 0)
      DrawLine((i - 1) * (RECT_SIZE + 2) + MIDDLE_RECT_POINT,
               state->ts_data[i - 1] + MIDDLE_RECT_POINT, x + MIDDLE_RECT_POINT,
               y + MIDDLE_RECT_POINT, BLACK);

    draw_rectangle_lines_ex(rec, BLACK);
  }
  for (int i = 0; i < state->data_count; i++) {
    if (!state->selected_data[i]) {
      continue;
    }

    float x = i * (RECT_SIZE + 2);
    float y = state->ts_data[i];

    Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};
    DrawRectangleRec(rec, YELLOW);
    draw_rectangle_lines_ex(rec, BLACK);
  }
}

void draw_timeseries(TimeClusterState *state) {
  // TODO draw everything and check if it has good performance
  // otherwise, try to scale/pan without BeginMode2D
  // lastly, try drawing texture
  for (int i = 0; i < state->data_count; i++) {
    float x = i * (RECT_SIZE + 2);
    float y = state->ts_data[i];
    Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};

    // TODO change to DrawLineStrip
    if (i > 0)
      DrawLine((i - 1) * (RECT_SIZE + 2) + MIDDLE_RECT_POINT,
               state->ts_data[i - 1] + MIDDLE_RECT_POINT, x + MIDDLE_RECT_POINT,
               y + MIDDLE_RECT_POINT, BLACK);

    draw_rectangle_lines_ex(rec, BLACK);
  }
  for (int i = 0; i < state->data_count; i++) {
    if (!state->selected_data[i]) {
      continue;
    }

    float x = i * (RECT_SIZE + 2);

    Rectangle rec = {x-1, 0, RECT_SIZE+2, 999999};
    DrawRectangleRec(rec, Fade(YELLOW, 0.5f));
  }
}

void draw_hud(TimeClusterState *state) {
  // char str[80];
  // for (int i = 0; i < state->data_count; i++) {
  //   sprintf(str, "%d: %d", i, state->selected_data[i]);
  //   DrawText(str, 0, i * 22, 20, RED);
  // }

  DrawLine(0, GetScreenHeight() / 2, GetScreenWidth(), GetScreenHeight() / 2,
           BLACK);
}

void handle_pan(Camera2D *camera, bool lock_y) {
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    Vector2 delta = GetMouseDelta();
    if (lock_y)
      delta.y = 0;
    delta = Vector2Scale(delta, -1.0f / camera->zoom);

    camera->target = Vector2Add(camera->target, delta);
  }
}

void handle_zoom(Camera2D *camera) {
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);

    camera->offset = GetMousePosition();

    camera->target = mouseWorldPos;

    const float zoomIncrement = 0.125f;

    camera->zoom += (wheel * zoomIncrement);
    if (camera->zoom < zoomIncrement)
      camera->zoom = zoomIncrement;
  }
}

void handle_selection(TimeClusterState *state, Camera2D camera) {
  Vector2 mousePosition = GetMousePosition();
  Vector2 worldMousePosition = GetScreenToWorld2D(mousePosition, camera);

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
      float x = i * (RECT_SIZE + 2);
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
  int mouse_in_panel = 0;
  if (GetMousePosition().y < (GetScreenHeight() / 2.0f)) {
    mouse_in_panel = 1;
  } else {
    mouse_in_panel = 2;
  }

  Camera2D *current_camera = NULL;
  if (mouse_in_panel == 1) {
    current_camera = &state->camera1;
  } else {
    current_camera = &state->camera2;
  }

  handle_pan(current_camera, mouse_in_panel == 1);
  handle_zoom(current_camera);
  handle_selection(state, *current_camera);

  BeginDrawing();
  {
    ClearBackground(WHITE);

    // first panel
    BeginScissorMode(0, 0, GetScreenWidth(), GetScreenHeight() / 2);
    {
      BeginMode2D(state->camera1);
      {
        if (mouse_in_panel == 1)
          draw_selection(state);
        draw_timeseries(state);
      }
      EndMode2D();
    }
    EndScissorMode();

    // second panel
    BeginScissorMode(0, GetScreenHeight() / 2, GetScreenWidth(),
                     GetScreenHeight());
    {
      BeginMode2D(state->camera2);
      {
        if (mouse_in_panel == 2)
          draw_selection(state);
        draw_dr_data(state);
      }
      EndMode2D();
    }
    EndScissorMode();

    draw_hud(state);
  }
  EndDrawing();
}

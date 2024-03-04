#include "render.h"

#include <python3.11/Python.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stddef.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

#define RECT_SIZE 8.0f
#define MIDDLE_RECT_POINT 4.0f
#define MOUSE_SCALE_MARK_SIZE 8
#define FOREGROUND BLACK
#define BACKGROUND LIGHTGRAY
#define PRIMARY RED
#define SECONDARY BLUE

TimeClusterState init() {
  SetTraceLogLevel(LOG_DEBUG);

  TraceLog(LOG_DEBUG, "Initializing");

  TimeClusterState state;
  state.data_count = 0;

  int screenWidth = 800;
  int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "TimeCluster");

  TraceLog(LOG_DEBUG, "Creating cameras");

  // TODO better default zoom
  state.camera1 = (Camera2D){
      .offset = {0, 0}, .target = {0, 0}, .rotation = 0, .zoom = 1.0f};

  state.camera2 = (Camera2D){
      .offset = {0, 0}, .target = {0, 0}, .rotation = 0, .zoom = 1.0f};

  state.selection_mode = false;
  state.selection = (Rectangle){0};

  SetTargetFPS(60);

  TraceLog(LOG_DEBUG, "Initialized");

  return state;
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
               y + MIDDLE_RECT_POINT, FOREGROUND);

    draw_rectangle_lines_ex(rec, FOREGROUND);
  }
  for (int i = 0; i < state->data_count; i++) {
    if (!state->selected_data[i]) {
      continue;
    }

    float x = i * (RECT_SIZE + 2);
    float y = state->ts_data[i];

    Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};
    DrawRectangleRec(rec, PRIMARY);
    draw_rectangle_lines_ex(rec, FOREGROUND);
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
               y + MIDDLE_RECT_POINT, FOREGROUND);

    // draw_rectangle_lines_ex(rec, FOREGROUND);
  }
  for (int i = 0; i < state->data_count; i++) {
    if (!state->selected_data[i]) {
      continue;
    }

    float x = i * (RECT_SIZE + 2);

    Rectangle rec = {x - 1, 0, RECT_SIZE + 2, 999999};
    DrawRectangleRec(rec, Fade(PRIMARY, 0.5f));
  }
}

void draw_hud(TimeClusterState *state) {
  DrawText(state->filePath, 0, 0, 20, BLACK);
  DrawLine(0, GetScreenHeight() / 2, GetScreenWidth(), GetScreenHeight() / 2,
           FOREGROUND);

  DrawFPS(GetScreenWidth() - MeasureText("60 FPS", 20) - 5, 5);
}

void handle_pan(Camera2D *camera) {
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f / camera->zoom);

    camera->target = Vector2Add(camera->target, delta);
  }
}

void handle_zoom(Camera2D *camera) {
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);

    // TODO fix this offset. Started after using offset for panel handling
    // camera->offset = GetMousePosition();

    camera->target = mouseWorldPos;

    const float zoomIncrement = 0.125f;

    camera->zoom += (wheel * zoomIncrement);
    if (camera->zoom < zoomIncrement)
      camera->zoom = zoomIncrement;
  }
}

// TODO inverted selection
void handle_selection(TimeClusterState *state, Camera2D *camera) {
  Vector2 mousePosition = GetMousePosition();
  Vector2 worldMousePosition = GetScreenToWorld2D(mousePosition, *camera);

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !state->selection_mode) {
    state->selection_mode = true;
    state->selection.x = worldMousePosition.x;
    state->selection.y = worldMousePosition.y;
  }

  if (state->selection_mode) {
    state->selection.width = (worldMousePosition.x - state->selection.x);
    state->selection.height = (worldMousePosition.y - state->selection.y);

    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
      state->selection_mode = false;

    for (int i = 0; i < state->data_count; i++) {
      float x = i * (RECT_SIZE + 2);
      float y = state->ts_data[i];
      Rectangle rec = {x, y, RECT_SIZE, RECT_SIZE};

      state->selected_data[i] = CheckCollisionRecs(state->selection, rec);
    }
  }
}

void draw_selection(TimeClusterState *state) {
  if (state->selection_mode) {
    DrawRectangleRec(state->selection, Fade(SECONDARY, 0.2f));

    draw_rectangle_lines_ex(state->selection, SECONDARY);
    // TODO draw a fixed sized triangle independent from camera.zoom
    // DrawTriangle((Vector2){state->selection.x + state->selection.width -
    //                            MOUSE_SCALE_MARK_SIZE,
    //                        state->selection.y + state->selection.height},
    //              (Vector2){state->selection.x + state->selection.width,
    //                        state->selection.y + state->selection.height},
    //              (Vector2){state->selection.x + state->selection.width,
    //                        state->selection.y + state->selection.height -
    //                            MOUSE_SCALE_MARK_SIZE},
    //              SECONDARY);
  }
}

void handle_hud(TimeClusterState *state) {
  // TODO resize panels
}

void load_file(TimeClusterState *state) {
  TraceLog(LOG_DEBUG, "Loading file");

  PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *pArgs;
  Py_Initialize();
  PyObject *sys = PyImport_ImportModule("sys");
  PyObject *path = PyObject_GetAttrString(sys, "path");
  PyList_Append(path, PyUnicode_FromString("."));
  pName = PyUnicode_FromString("py_function");
  pModule = PyImport_Import(pName);
  if (!pModule) {
    PyErr_Print();
    printf("ERROR in pModule\n");
    exit(1);
  }
  pDict = PyModule_GetDict(pModule);
  pFunc = PyDict_GetItemString(pDict, "multiply");
  if (PyCallable_Check(pFunc)) {
    // Prepare the argument list for the call
    pArgs = PyTuple_New(2);
    pValue = PyLong_FromLong(6);
    if (!pValue) {
      PyErr_Print();
      return;
    }
    PyTuple_SetItem(pArgs, 0, pValue);

    pValue = PyLong_FromLong(7);
    if (!pValue) {
      PyErr_Print();
      return;
    }
    PyTuple_SetItem(pArgs, 1, pValue);

    pValue = PyObject_CallObject(pFunc, pArgs);

    if (pArgs != NULL) {
      Py_DECREF(pArgs);
    }

    if (pValue != NULL) {
      int size = PySequence_Size(pValue);
      int values[size];

      for (int i = 0; i < size; i++) {
        PyObject *value = PySequence_GetItem(pValue, i);
        if (value != NULL) {
          printf("Return of call : %d\n", PyLong_AsLong(value));
        }
      }
      Py_DECREF(pValue);
    } else {
      PyErr_Print();
    }
  }
  // Clean up
  Py_DECREF(pModule);
  Py_DECREF(pName);
  Py_Finalize();

  int data_count = 4;
  float ts_data[] = {113677.0f, 113657.0f, 113644.0f, 113664.0f};

  state->ts_data = malloc(data_count * sizeof(float));
  state->dr_data = malloc(data_count * sizeof(Vector2));
  state->selected_data = malloc(data_count * sizeof(bool));
  state->data_count = data_count;

  for (int i = 0; i < data_count; i++) {
    state->ts_data[i] = ts_data[i];
    state->selected_data[i] = false;
  }
  TraceLog(LOG_DEBUG, "File loaded");

  TraceLog(LOG_DEBUG, "Calculating min, max and half");
  float min = 9999999;
  float max = 0;
  for (int i = 0; i < state->data_count; i++) {
    if (state->ts_data[i] < min) {
      min = state->ts_data[i];
    }

    if (state->ts_data[i] > max) {
      max = state->ts_data[i];
    }
  }
  float half = (max - min) / 2;

  TraceLog(LOG_DEBUG, "Updating camera");
  state->camera1.target = (Vector2){0, min + half};
  state->camera2.target = (Vector2){0, min + half};
}

void handle_drop(TimeClusterState *state) {
  if (IsFileDropped()) {
    TraceLog(LOG_DEBUG, "File dropped");
    FilePathList droppedFiles = LoadDroppedFiles();

    if (droppedFiles.count > 1) {
      TraceLog(LOG_ERROR, "Too many files!");
      UnloadDroppedFiles(droppedFiles);
      return;
    }
    TextCopy(state->filePath, droppedFiles.paths[0]);
    load_file(state);
    UnloadDroppedFiles(droppedFiles);
  }
}

void handle_resize(TimeClusterState *state) {
  state->camera1.offset = (Vector2){0, GetScreenHeight() / 4};
  state->camera2.offset = (Vector2){0, GetScreenHeight() / 4 * 3};
}

void render(TimeClusterState *state) {
  // select current camera
  int mouse_in_panel = 0;
  Camera2D *current_camera = NULL;
  if (GetMousePosition().y < (GetScreenHeight() / 2.0f)) {
    mouse_in_panel = 1;
    current_camera = &state->camera1;
  } else {
    mouse_in_panel = 2;
    current_camera = &state->camera2;
  }

  // update
  handle_pan(current_camera);
  handle_zoom(current_camera);
  handle_selection(state, current_camera);
  handle_hud(state);
  handle_drop(state);
  handle_resize(state);

  // draw
  BeginDrawing();
  {
    ClearBackground(BACKGROUND);

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

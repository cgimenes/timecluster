#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <raylib.h>

#include <dlfcn.h>

#include "draw.h"

init_t init = NULL;
draw_t draw = NULL;
const char *libdraw_file_name = "libdraw.so";
const char *libdraw_file_path = "build/libdraw.so";
time_t libdraw_file_mod_time = 0;
void *libdraw = NULL;

bool reload_lib() {
  if (libdraw != NULL)
    dlclose(libdraw);

  libdraw = dlopen(libdraw_file_name, RTLD_NOW);
  if (libdraw == NULL) {
    fprintf(stderr, "ERROR: could not load %s: %s", libdraw_file_name,
            dlerror());
    return false;
  }

  init = dlsym(libdraw, "init");
  if (init == NULL) {
    fprintf(stderr, "ERROR: could not find init symbol in %s: %s",
            libdraw_file_name, dlerror());
    return 1;
  }

  draw = dlsym(libdraw, "draw");
  if (draw == NULL) {
    fprintf(stderr, "ERROR: could not find draw symbol in %s: %s",
            libdraw_file_name, dlerror());
    return false;
  }
  libdraw_file_mod_time = GetFileModTime(libdraw_file_path);

  return true;
}

bool hot_reload() {
  time_t current_libdraw_file_mod_time = GetFileModTime(libdraw_file_path);
  if (current_libdraw_file_mod_time != libdraw_file_mod_time) {
    // mod time is changed quicker than the compilation, so we have an empty
    // file for an while. I may change it for a "trigger file" in the future.
    // Creating a specific file after compilation finishes and checking its mod
    // time here
    sleep(1);
    return reload_lib();
  }
  return true;
}

int main() {
  if (!reload_lib())
    return 1;

  TimeClusterState state = {
      .ts_data =
          {
              113924, 113764, 113757, 113690, 113610, 113523, 113604, 113564,
              113570, 113584, 113610, 113550, 113710, 113697, 113610, 113564,
              113564, 113557, 113510, 113497, 113570, 113597, 113677, 113604,
              113557, 113617, 113664, 113731, 113824, 113777, 113724, 113764,
              113717, 113824, 113864, 113797, 113797, 113784, 113764, 113724,
              113677, 113624, 113717, 113751, 113771, 113751, 113717, 113724,
              113751, 113737, 113717, 113717, 113744, 113797, 113804, 113744,
              113784, 113757, 113757, 113797, 113664, 113644, 113744, 113657,
              113644, 113704, 113724, 113697, 113737, 113704, 113677, 113684,
              113704, 113657, 113637, 113650, 113657, 113710, 113704, 113684,
              113744, 113837, 113944, 113964, 113931, 113924, 114064, 113984,
              113978, 113964, 113991, 113958, 113877, 113751, 113704, 113657,
              113677, 113657, 113644, 113664,
          },
      .dr_data = {},
      .data_count = 100};
  init(&state);

  while (!WindowShouldClose()) {
    if (!hot_reload())
      return 1;
    // 1:53

    draw(&state);
  }

  CloseWindow();
  return 0;
}

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <raylib.h>

#include <dlfcn.h>

#include "render.h"

init_t init = NULL;
render_t render = NULL;
const char *librender_file_name = "librender.so";
const char *librender_file_path = "build/librender.so";
time_t librender_file_mod_time = 0;
void *librender = NULL;

bool reload_lib() {
  if (librender != NULL)
    dlclose(librender);

  librender = dlopen(librender_file_name, RTLD_NOW);
  if (librender == NULL) {
    fprintf(stderr, "ERROR: could not load %s: %s", librender_file_name,
            dlerror());
    return false;
  }

  init = dlsym(librender, "init");
  if (init == NULL) {
    fprintf(stderr, "ERROR: could not find init symbol in %s: %s",
            librender_file_name, dlerror());
    return 1;
  }

  render = dlsym(librender, "render");
  if (render == NULL) {
    fprintf(stderr, "ERROR: could not find render symbol in %s: %s",
            librender_file_name, dlerror());
    return false;
  }
  librender_file_mod_time = GetFileModTime(librender_file_path);

  return true;
}

bool hot_reload() {
  time_t current_librender_file_mod_time = GetFileModTime(librender_file_path);
  if (current_librender_file_mod_time != librender_file_mod_time) {
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

  TimeClusterState state = init();

  while (!WindowShouldClose()) {
    if (!hot_reload())
      return 1;

    render(&state);
  }

  CloseWindow();
  return 0;
}

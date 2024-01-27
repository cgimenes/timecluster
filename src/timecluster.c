#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <raylib.h>

#include <dlfcn.h>

#include "draw.h"

init_t init = NULL;
draw_t draw = NULL;
const char *libdraw_file_name = "libdraw.so";
const char *libdraw_file_path = "build/libdraw.so";
time_t libdraw_file_mod_time = 0;
void *libdraw = NULL;

bool reload_lib()
{
    if (libdraw != NULL) dlclose(libdraw);

    libdraw = dlopen(libdraw_file_name, RTLD_NOW);
    if (libdraw == NULL) {
        fprintf(stderr, "ERROR: could not load %s: %s", libdraw_file_name, dlerror());
        return false;
    }

    init = dlsym(libdraw, "init");
    if (init == NULL) {
        fprintf(stderr, "ERROR: could not find init symbol in %s: %s", libdraw_file_name, dlerror());
        return 1;
    }

    draw = dlsym(libdraw, "draw");
    if (draw == NULL) {
        fprintf(stderr, "ERROR: could not find draw symbol in %s: %s", libdraw_file_name, dlerror());
        return false;
    }
    libdraw_file_mod_time = GetFileModTime(libdraw_file_path);

    return true;
}

bool hot_reload()
{
    time_t current_libdraw_file_mod_time = GetFileModTime(libdraw_file_path);
    if (current_libdraw_file_mod_time != libdraw_file_mod_time) {
        // mod time is changed quicker than the compilation, so we have an empty file for an while.
        // I may change it for a "trigger file" in the future. Creating a specific file after
        // compilation finishes and checking its mod time here
        sleep(1);
        return reload_lib();
    }
    return true;
}

int main()
{
    if (!reload_lib()) return 1;

    TimeClusterState state;
    init(&state);

    while (!WindowShouldClose())
    {
        if (!hot_reload()) return 1;
        // 1:53

        draw(&state);
    }

    CloseWindow();
    return 0;
}
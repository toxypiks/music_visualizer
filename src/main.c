#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <raylib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include "plug.h"
#include <dlfcn.h>

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

const char *libplug_file_name = "libplug.so";
void *libplug = NULL;

#ifdef HOTRELOAD
//define LIST_OF_PLUGS from plug.h
//concatenate token passed to name also to name_t using name##_t
#define PLUG(name, ...) name##_t *name = NULL;
LIST_OF_PLUGS
#else
#define PLUG(name, ...) name##_t name;
LIST_OF_PLUGS
#endif
#undef PLUG

#ifdef HOTRELOAD
bool reload_libplug(void)
{
  if(libplug != NULL) dlclose(libplug);

  libplug = dlopen(libplug_file_name, RTLD_NOW);
  if (libplug == NULL) {
    fprintf(stderr, "ERROR: could not load %s: %s", libplug_file_name, dlerror());
    return false;
  }

  //define string literal with # in macro definition
#define PLUG(name, ...)					\
      name = dlsym(libplug, #name); \
      if (name == NULL) { \
        fprintf(stderr, "ERROR: could not find %s symbol in %s: %s",    \
                #name, libplug_file_name, dlerror());                   \
        return false;                                                   \
      }
  LIST_OF_PLUGS
  #undef PLUG

  return true;
}

#else
#define reload_libplug() true
#endif

int main(void)
{

  if (!reload_libplug()) return 1;

  size_t factor = 60;
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(factor*16, factor*9, "Musializer");
  SetTargetFPS(60);
  InitAudioDevice();

  plug_init();

  while(!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      void *state = plug_pre_reload();
      if (!reload_libplug()) return 1;
      plug_post_reload(state);
    }
    plug_update();
  }
  return 0;
}

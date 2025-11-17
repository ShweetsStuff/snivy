#include <cstdlib>

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif

#include "loader.h"
#include "state.h"

using namespace game;

#ifdef __EMSCRIPTEN__
static void emscripten_loop(void* arg)
{
  auto* state = (State*)(arg);
  state->loop();

  if (!state->isRunning) emscripten_cancel_main_loop();
}
#endif

int main()
{
  Loader loader;

  if (loader.isError) return EXIT_FAILURE;

  State state(loader.window, loader.context, Loader::SIZE);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(emscripten_loop, &state, 0, true);
#else
  while (state.isRunning)
    state.loop();
#endif

  return EXIT_SUCCESS;
}

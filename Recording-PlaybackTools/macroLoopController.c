#include "macroLoopController.h"

bool macroLoopControllerConfigure(MacroPlaybackState *state,
                                  uint64_t repeat_count) {
  if (state == NULL || state->active)
    return false;
  state->loops_total = repeat_count;
  state->loops_completed = 0;
  return true;
}
void macroLoopControllerReset(MacroPlaybackState *state) {
  if (state != NULL)
    state->loops_completed = 0;
}
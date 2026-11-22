#include "macroStopController.h"

bool macroStopControllerRequest(MacroPlaybackState *s) {
  if (!s || !s->active)
    return false;
  macroPlaybackRequestStop(s);
  return true;
}
bool macroStopControllerIsRequested(const MacroPlaybackState *s) {
  return s != NULL && s->stop_requested;
}
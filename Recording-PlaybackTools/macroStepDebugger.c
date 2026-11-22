#include "macroStepDebugger.h"

bool macroStepDebuggerStep(MacroPlaybackState *s, uint64_t now) {
  if (!s || !s->active || !s->paused)
    return false;
  s->single_step = true;
  return macroPlaybackResume(s, now);
}
bool macroStepDebuggerContinue(MacroPlaybackState *s, uint64_t now) {
  if (!s || !s->active)
    return false;
  s->single_step = false;
  if (s->paused)
    return macroPlaybackResume(s, now);
  return true;
}
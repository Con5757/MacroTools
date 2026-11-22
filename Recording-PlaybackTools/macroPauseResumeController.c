#include "macroPauseResumeController.h"

bool macroPauseResumeControllerPause(MacroPlaybackState *s, uint64_t n) {
  return macroPlaybackPause(s, n);
}
bool macroPauseResumeControllerResume(MacroPlaybackState *s, uint64_t n) {
  return macroPlaybackResume(s, n);
}
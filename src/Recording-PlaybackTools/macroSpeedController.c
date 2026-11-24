#include "macroSpeedController.h"

bool macroSpeedControllerSet(MacroPlaybackState *s, double m) {
  return macroPlaybackSetSpeed(s, m);
}
double macroSpeedControllerGet(const MacroPlaybackState *s) {
  return s ? s->speed : 0.0;
}
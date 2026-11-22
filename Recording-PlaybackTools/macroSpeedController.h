#pragma once
#include "macroToolsCommon.h"

bool macroSpeedControllerSet(MacroPlaybackState *state, double multiplier);
double macroSpeedControllerGet(const MacroPlaybackState *state);
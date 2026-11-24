#pragma once
#include "macroToolsCommon.h"

// repeat_count is the total plays counter -> 0 = inf
bool macroLoopControllerConfigure(MacroPlaybackState *state,
                                  uint64_t repeat_count);
void macroLoopControllerReset(MacroPlaybackState *state);
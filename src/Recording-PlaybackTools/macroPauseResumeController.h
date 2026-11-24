#pragma once
#include "macroToolsCommon.h"

bool macroPauseResumeControllerPause(MacroPlaybackState *state,
                                     uint64_t now_ms);
bool macroPauseResumeControllerResume(MacroPlaybackState *state,
                                      uint64_t now_ms);
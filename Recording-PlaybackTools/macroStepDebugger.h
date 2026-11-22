#pragma once
#include "macroToolsCommon.h"

bool macroStepDebuggerStep(MacroPlaybackState *state, uint64_t now_ms);
bool macroStepDebuggerContinue(MacroPlaybackState *state, uint64_t now_ms);
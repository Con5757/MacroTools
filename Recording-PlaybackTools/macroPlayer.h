#pragma once
#include "macroToolsCommon.h"

void macroPlayerInitialize(MacroPlaybackState *state);
bool macroPlayerStart(MacroPlaybackState *state, const MacroSequence *sequence,
                      uint64_t now_ms, MacroDispatchFunction dispatch,
                      void *dispatch_context);
void macroPlayerSetConditionHandler(MacroPlaybackState *state,
                                    MacroConditionFunction condition,
                                    void *context);
MacroPlayerResult macroPlayerUpdate(MacroPlaybackState *state, uint64_t now_ms,
                                    size_t max_steps_per_update,
                                    size_t *steps_processed);
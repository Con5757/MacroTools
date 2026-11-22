#pragma once
#include "macroToolsCommon.h"

typedef struct {
  size_t step_index;
  uint64_t loops_completed;
  uint64_t remaining_delay_ms;
  double speed;
  bool valid;
} MacroCheckpoint;

bool macroCheckpointManagerSave(const MacroPlaybackState *state,
                                uint64_t now_ms, MacroCheckpoint *checkpoint);
bool macroCheckpointManagerRestore(MacroPlaybackState *state, uint64_t now_ms,
                                   const MacroCheckpoint *checkpoint);
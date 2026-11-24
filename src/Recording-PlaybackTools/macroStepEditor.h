#pragma once
#include "macroToolsCommon.h"

bool macroStepEditorSetDelay(MacroSequence *sequence, size_t index,
                             uint64_t delay_ms);
bool macroStepEditorSetMousePosition(MacroSequence *sequence, size_t index,
                                     int32_t x, int32_t y);
bool macroStepEditorSetCodeValue(MacroSequence *sequence, size_t index,
                                 int32_t code, int32_t value);
bool macroStepEditorSetText(MacroSequence *sequence, size_t index,
                            const char *text);
bool macroStepEditorSetBranch(MacroSequence *sequence, size_t index,
                              uint32_t true_target, uint32_t false_target);
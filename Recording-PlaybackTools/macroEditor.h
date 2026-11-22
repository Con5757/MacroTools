#pragma once
#include "macroToolsCommon.h"

bool macroEditorInsertStep(MacroSequence *sequence, size_t index,
                           const MacroStep *step);
bool macroEditorDeleteStep(MacroSequence *sequence, size_t index);
bool macroEditorMoveStep(MacroSequence *sequence, size_t from_index,
                         size_t to_index);
bool macroEditorDuplicateStep(MacroSequence *sequence, size_t index,
                              size_t insert_at);
#pragma once
#include "macroToolsCommon.h"

bool macroBranchingResolve(const MacroSequence *sequence, size_t current_index,
                           const MacroVariableStore *variables,
                           size_t *next_index);
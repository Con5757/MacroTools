#pragma once
#include "macroToolsCommon.h"

bool macroConditionManagerEvaluate(const MacroStep *condition_step,
                                   const MacroVariableStore *variables,
                                   bool *result);
#pragma once
#include "macroToolsCommon.h"

typedef MacroVariableStore MacroVariableManager;

void macroVariableManagerInitialize(MacroVariableManager *manager);
void macroVariableManagerDestroy(MacroVariableManager *manager);
bool macroVariableManagerSet(MacroVariableManager *manager, const char *name,
                             double value);
bool macroVariableManagerGet(const MacroVariableManager *manager,
                             const char *name, double *value);
bool macroVariableManagerRemove(MacroVariableManager *manager,
                                const char *name);
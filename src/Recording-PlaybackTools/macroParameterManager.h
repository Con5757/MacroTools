#pragma once
#include "macroToolsCommon.h"

typedef struct {
  char name[MACRO_NAME_CAPACITY];
  double default_value;
  double value;
  bool has_override;
} MacroParameter;
typedef struct {
  MacroParameter *items;
  size_t count;
  size_t capacity;
} MacroParameterManager;

void macroParameterManagerInitialize(MacroParameterManager *manager);
void macroParameterManagerDestroy(MacroParameterManager *manager);
bool macroParameterManagerDeclare(MacroParameterManager *manager,
                                  const char *name, double default_value);
bool macroParameterManagerSet(MacroParameterManager *manager, const char *name,
                              double value);
bool macroParameterManagerGet(const MacroParameterManager *manager,
                              const char *name, double *value);
void macroParameterManagerResetOverrides(MacroParameterManager *manager);
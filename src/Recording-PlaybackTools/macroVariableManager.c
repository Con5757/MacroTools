#include "macroVariableManager.h"

void macroVariableManagerInitialize(MacroVariableManager *m) {
  macroVariableStoreInitialize(m);
}
void macroVariableManagerDestroy(MacroVariableManager *m) {
  macroVariableStoreDestroy(m);
}
bool macroVariableManagerSet(MacroVariableManager *m, const char *n, double v) {
  return macroVariableStoreSet(m, n, v);
}
bool macroVariableManagerGet(const MacroVariableManager *m, const char *n,
                             double *v) {
  return macroVariableStoreGet(m, n, v);
}
bool macroVariableManagerRemove(MacroVariableManager *m, const char *n) {
  return macroVariableStoreRemove(m, n);
}
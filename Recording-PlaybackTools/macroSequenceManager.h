#pragma once
#include "macroToolsCommon.h"

typedef struct {
  MacroSequence **items; // caller owns the sequence for pointers without an owner
  size_t count;
  size_t capacity;
} MacroSequenceManager;

void macroSequenceManagerInitialize(MacroSequenceManager *manager);
void macroSequenceManagerDestroy(MacroSequenceManager *manager);
bool macroSequenceManagerAdd(MacroSequenceManager *manager,
                             MacroSequence *sequence);
bool macroSequenceManagerRemove(MacroSequenceManager *manager,
                                const MacroSequence *sequence);
MacroSequence *macroSequenceManagerFind(MacroSequenceManager *manager,
                                        const char *name);
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct {
  ULONGLONG keyDownCount;
  ULONGLONG repeatCount;
  ULONGLONG lastRepeatIntervalMs;
  ULONGLONG totalRepeatIntervalMs;
  double averageRepeatIntervalMs;
  double repeatsPerSecond;
} KeyRepeatStats;

typedef struct {
  HHOOK hook;
  BOOL isRunning;
  BOOL includeInjected;
  BYTE keyHeld[256];
  ULONGLONG lastKeyDownAt[256];
  KeyRepeatStats stats[256];
} KeyRepeatAnalyzer;

BOOL keyRepeatAnalyzerInitialize(KeyRepeatAnalyzer *analyzer);
BOOL keyRepeatAnalyzerStart(KeyRepeatAnalyzer *analyzer, BOOL includeInjected);
void keyRepeatAnalyzerStop(KeyRepeatAnalyzer *analyzer);
BOOL keyRepeatAnalyzerGetStats(const KeyRepeatAnalyzer *analyzer,
                               WORD virtualKey, KeyRepeatStats *stats);

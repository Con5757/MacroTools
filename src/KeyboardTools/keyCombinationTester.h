#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stddef.h>

#define KEY_COMBINATION_TESTER_MAX_KEYS 16

typedef struct {
  WORD keys[KEY_COMBINATION_TESTER_MAX_KEYS];
  BYTE keyDown[KEY_COMBINATION_TESTER_MAX_KEYS];
  size_t keyCount;
  BOOL wasCombinationDown;
  BOOL initialized;
} KeyCombinationTester;

typedef struct {
  BOOL allDown;
  BOOL anyDown;
  BOOL pressed;
  BOOL released;
  BYTE keyDown[KEY_COMBINATION_TESTER_MAX_KEYS];
  size_t keyCount;
  ULONGLONG timestampMs;
} KeyCombinationTesterData;

BOOL keyCombinationTesterInitialize(KeyCombinationTester *tester,
                                    const WORD *virtualKeys, size_t keyCount);
BOOL keyCombinationTesterUpdate(KeyCombinationTester *tester,
                                KeyCombinationTesterData *data);
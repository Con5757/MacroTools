#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct {
  WORD virtualKey;
  BOOL previousDown;
  BOOL initialized;
} KeyTester;

typedef struct {
  WORD virtualKey;
  BOOL isDown;
  BOOL pressed;
  BOOL released;
  ULONGLONG timestampMs;
} KeyTesterData;

BOOL keyTesterInitialize(KeyTester *tester, WORD virtualKey);
BOOL keyTesterUpdate(KeyTester *tester, KeyTesterData *data);
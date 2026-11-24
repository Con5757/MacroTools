#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "mouseButtonMonitor.h"
#include <Windows.h>

typedef struct {
  DWORD heldButtons;
  DWORD pressedThisUpdate;
  DWORD releasedThisUpdate;
  ULONGLONG pressCounts[MOUSE_BUTTON_COUNT];
  ULONGLONG releaseCounts[MOUSE_BUTTON_COUNT];
  MouseButton lastChangedButton;
  BOOL lastChangeWasPress;
  POINT lastChangePosition;
  BOOL hasLastChange;
} MouseClickTester;

BOOL mouseClickTesterInitialize(MouseClickTester *tester);
BOOL mouseClickTesterUpdate(MouseClickTester *tester, DWORD *pressedThisUpdate,
                            DWORD *releasedThisUpdate);
#define WIN32_LEAN_AND_MEAN
#include "mouseClickTester.h"

BOOL mouseClickTesterInitialize(MouseClickTester *tester) {
  if (tester == NULL) {
    return FALSE;
  }
  ZeroMemory(tester, sizeof(*tester));
  tester->lastChangedButton = MOUSE_BUTTON_LEFT;
  tester->heldButtons = mouseButtonMonitorGetState();
  return TRUE;
}

BOOL mouseClickTesterUpdate(MouseClickTester *tester, DWORD *pressedThisUpdate,
                            DWORD *releasedThisUpdate) {
  if (tester == NULL) {
    return FALSE;
  }

  DWORD current = mouseButtonMonitorGetState();
  DWORD changed = current ^ tester->heldButtons;
  DWORD pressed = changed & current;
  DWORD released = changed & tester->heldButtons;

  if (changed != 0) {
    POINT position;
    if (!GetCursorPos(&position)) {
      return FALSE;
    }

    for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
      DWORD mask = (DWORD)(1u << i);
      if ((pressed & mask) != 0) {
        tester->pressCounts[i]++;
        tester->lastChangedButton = (MouseButton)i;
        tester->lastChangeWasPress = TRUE;
        tester->lastChangePosition = position;
        tester->hasLastChange = TRUE;
      }
      if ((released & mask) != 0) {
        tester->releaseCounts[i]++;
        tester->lastChangedButton = (MouseButton)i;
        tester->lastChangeWasPress = FALSE;
        tester->lastChangePosition = position;
        tester->hasLastChange = TRUE;
      }
    }
  }

  tester->heldButtons = current;
  if (pressedThisUpdate != NULL) {
    *pressedThisUpdate = pressed;
  }
  if (releasedThisUpdate != NULL) {
    *releasedThisUpdate = released;
  }

  return TRUE;
}

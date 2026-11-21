#define WIN32_LEAN_AND_MEAN
#include "keyTester.h"

BOOL keyTesterInitialize(KeyTester *tester, WORD virtualKey) {
  if (tester == NULL || virtualKey == 0 || virtualKey > 255)
    return FALSE;
  tester->virtualKey = virtualKey;
  tester->previousDown = FALSE;
  tester->initialized = TRUE;
  return TRUE;
}

BOOL keyTesterUpdate(KeyTester *tester, KeyTesterData *data) {
  if (tester == NULL || data == NULL || !tester->initialized)
    return FALSE;

  BOOL down = (GetAsyncKeyState((int)tester->virtualKey) & 0x8000) != 0;
  data->virtualKey = tester->virtualKey;
  data->isDown = down;
  data->pressed = down && !tester->previousDown;
  data->released = !down && tester->previousDown;
  data->timestampMs = GetTickCount64();
  tester->previousDown = down;
  return TRUE;
}

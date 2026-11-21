#define WIN32_LEAN_AND_MEAN
#include "keyboardStateSnapshot.h"

BOOL keyboardStateSnapshotCapture(KeyboardStateSnapshot *snapshot) {
  if (snapshot == NULL)
    return FALSE;

  for (unsigned int key = 0; key < KEYBOARD_STATE_SNAPSHOT_KEY_COUNT; ++key) {
    snapshot->keyDown[key] = (GetAsyncKeyState((int)key) & 0x8000) != 0;
  }
  snapshot->capturedAtMs = GetTickCount64();
  return TRUE;
}

BOOL keyboardStateSnapshotIsDown(const KeyboardStateSnapshot *snapshot,
                                 WORD virtualKey) {
  if (snapshot == NULL || virtualKey >= KEYBOARD_STATE_SNAPSHOT_KEY_COUNT) {
    return FALSE;
  }
  return snapshot->keyDown[virtualKey] != 0;
}
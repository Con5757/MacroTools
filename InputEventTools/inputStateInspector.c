#include "inputStateInspector.h"
#include <string.h>

BOOL inputStateInspectorCapture(InputStateSnapshot *snapshot) {
  if (snapshot == NULL)
    return FALSE;
  memset(snapshot, 0, sizeof(*snapshot));
  for (UINT key = 0; key < 256; ++key)
    snapshot->keys[key] = (GetAsyncKeyState((int)key) & 0x8000) ? 1 : 0;
  snapshot->mouseButtons[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
  snapshot->mouseButtons[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
  snapshot->mouseButtons[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
  snapshot->mouseButtons[3] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
  snapshot->mouseButtons[4] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
  if (!GetCursorPos(&snapshot->cursorPosition))
    return FALSE;
  snapshot->timestampMs = GetTickCount64();
  return TRUE;
}
BOOL inputStateInspectorIsKeyDown(const InputStateSnapshot *snapshot,
                                  UINT virtualKey) {
  return snapshot != NULL && virtualKey < 256 &&
         snapshot->keys[virtualKey] != 0;
}
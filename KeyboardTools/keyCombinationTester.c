#define WIN32_LEAN_AND_MEAN
#include "keyCombinationTester.h"

#include <string.h>

BOOL keyCombinationTesterInitialize(KeyCombinationTester *tester,
                                    const WORD *virtualKeys, size_t keyCount) {
  if (tester == NULL || virtualKeys == NULL || keyCount == 0 ||
      keyCount > KEY_COMBINATION_TESTER_MAX_KEYS)
    return FALSE;

  ZeroMemory(tester, sizeof(*tester));
  for (size_t i = 0; i < keyCount; ++i) {
    if (virtualKeys[i] == 0 || virtualKeys[i] > 255)
      return FALSE;
    for (size_t j = 0; j < i; ++j) {
      if (virtualKeys[i] == virtualKeys[j])
        return FALSE;
    }
    tester->keys[i] = virtualKeys[i];
  }
  tester->keyCount = keyCount;
  tester->initialized = TRUE;
  return TRUE;
}

BOOL keyCombinationTesterUpdate(KeyCombinationTester *tester,
                                KeyCombinationTesterData *data) {
  if (tester == NULL || data == NULL || !tester->initialized)
    return FALSE;

  ZeroMemory(data, sizeof(*data));
  data->allDown = TRUE;
  data->keyCount = tester->keyCount;

  for (size_t i = 0; i < tester->keyCount; ++i) {
    BOOL down = (GetAsyncKeyState((int)tester->keys[i]) & 0x8000) != 0;
    tester->keyDown[i] = down ? 1 : 0;
    data->keyDown[i] = down ? 1 : 0;
    data->anyDown = data->anyDown || down;
    data->allDown = data->allDown && down;
  }

  data->pressed = data->allDown && !tester->wasCombinationDown;
  data->released = !data->allDown && tester->wasCombinationDown;
  data->timestampMs = GetTickCount64();
  tester->wasCombinationDown = data->allDown;
  return TRUE;
}

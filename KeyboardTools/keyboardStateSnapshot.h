#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define KEYBOARD_STATE_SNAPSHOT_KEY_COUNT 256

typedef struct {
  BYTE keyDown[KEYBOARD_STATE_SNAPSHOT_KEY_COUNT];
  ULONGLONG capturedAtMs;
} KeyboardStateSnapshot;

BOOL keyboardStateSnapshotCapture(KeyboardStateSnapshot *snapshot);
BOOL keyboardStateSnapshotIsDown(const KeyboardStateSnapshot *snapshot,
                                 WORD virtualKey);
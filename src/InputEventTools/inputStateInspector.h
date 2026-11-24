#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputStateSnapshot {
  BYTE keys[256]; // should be 1 when the bit of GetAsyncKeyState is set
  BOOL mouseButtons[5];
  POINT cursorPosition;
  ULONGLONG timestampMs;
} InputStateSnapshot;

BOOL inputStateInspectorCapture(InputStateSnapshot *snapshot);
BOOL inputStateInspectorIsKeyDown(const InputStateSnapshot *snapshot,
                                  UINT virtualKey);

#ifdef __cplusplus
}
#endif
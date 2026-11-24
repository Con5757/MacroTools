#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputSimulationResult {
  UINT requested;
  UINT sent;
  ULONGLONG timestampMs;
  DWORD lastError;
} InputSimulationResult;

BOOL inputSimulationTesterSendKey(UINT virtualKey, BOOL keyDown,
                                  InputSimulationResult *result);
BOOL inputSimulationTesterSendMouseButton(UINT button, BOOL buttonDown,
                                          InputSimulationResult *result);
BOOL inputSimulationTesterMoveMouse(LONG dx, LONG dy,
                                    InputSimulationResult *result);
BOOL inputSimulationTesterSendWheel(LONG delta, BOOL horizontal,
                                    InputSimulationResult *result);

#ifdef __cplusplus
}
#endif
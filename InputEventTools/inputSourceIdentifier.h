#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

InputEventSource inputSourceIdentifierClassify(
    const InputEvent
        *event); // raw input handles can identify device backed input
                 // hook flags should identify any injected input
void inputSourceIdentifierApplyKeyboardHookFlags(InputEvent *event,
                                                 DWORD hookFlags);
void inputSourceIdentifierApplyMouseHookFlags(InputEvent *event,
                                              DWORD hookFlags);

#ifdef __cplusplus
}
#endif
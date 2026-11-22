#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputEventFilter {
  DWORD typeMask;          // Bit (1u << InputEventType); 0 means all types
  HANDLE deviceHandle;     // NULL: any device
  UINT virtualKey;         // 0: any key
  UINT mouseButton;        // 0: any button
  InputEventSource source; // UNKNOWN: treated as any source
} InputEventFilter;

void inputEventFilterInitialize(InputEventFilter *filter);
BOOL inputEventFilterMatches(const InputEventFilter *filter,
                             const InputEvent *event);
size_t inputEventFilterCopy(const InputEventFilter *filter,
                            const InputEvent *input, size_t inputCount,
                            InputEvent *output, size_t outputCapacity);

#ifdef __cplusplus
}
#endif
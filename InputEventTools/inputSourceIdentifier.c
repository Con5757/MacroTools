#include "inputSourceIdentifier.h"
InputEventSource inputSourceIdentifierClassify(const InputEvent *event) {
    if (event == NULL) return INPUT_EVENT_SOURCE_UNKNOWN;
    return inputEventClassifySource(event->deviceHandle, event->source == INPUT_EVENT_SOURCE_INJECTED);
}
void inputSourceIdentifierApplyKeyboardHookFlags(InputEvent *event, DWORD hookFlags) {
    if (event == NULL) return;
    if ((hookFlags & (LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED)) != 0)
        event->source = INPUT_EVENT_SOURCE_INJECTED;
    else if (event->deviceHandle != NULL)
        event->source = INPUT_EVENT_SOURCE_PHYSICAL;
    else
        event->source = INPUT_EVENT_SOURCE_UNKNOWN;
    event->extraInfo = hookFlags;
}
void inputSourceIdentifierApplyMouseHookFlags(InputEvent *event, DWORD hookFlags) {
    if (event == NULL) return;
    if ((hookFlags & (LLMHF_INJECTED | LLMHF_LOWER_IL_INJECTED)) != 0)
        event->source = INPUT_EVENT_SOURCE_INJECTED;
    else if (event->deviceHandle != NULL)
        event->source = INPUT_EVENT_SOURCE_PHYSICAL;
    else
        event->source = INPUT_EVENT_SOURCE_UNKNOWN;
    event->extraInfo = hookFlags;
}
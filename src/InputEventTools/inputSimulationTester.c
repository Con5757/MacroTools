#include "inputSimulationTester.h"
#include <string.h>
static BOOL submit(INPUT *input, InputSimulationResult *result) {
    if (result == NULL) return FALSE;
    memset(result, 0, sizeof(*result));
    result->requested = 1;
    result->timestampMs = GetTickCount64();
    result->sent = SendInput(1, input, sizeof(*input));
    if (result->sent != 1) result->lastError = GetLastError();
    return result->sent == 1;
}
BOOL inputSimulationTesterSendKey(UINT virtualKey, BOOL keyDown, InputSimulationResult *result) {
    if (virtualKey == 0 || virtualKey > 0xFF) return FALSE;
    INPUT input; memset(&input, 0, sizeof(input));
    input.type = INPUT_KEYBOARD; input.ki.wVk = (WORD)virtualKey;
    if (!keyDown) input.ki.dwFlags = KEYEVENTF_KEYUP;
    return submit(&input, result);
}
BOOL inputSimulationTesterSendMouseButton(UINT button, BOOL buttonDown, InputSimulationResult *result) {
    INPUT input; memset(&input, 0, sizeof(input)); input.type = INPUT_MOUSE;
    switch (button) {
        case 1: input.mi.dwFlags = buttonDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP; break;
        case 2: input.mi.dwFlags = buttonDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP; break;
        case 3: input.mi.dwFlags = buttonDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP; break;
        case 4: input.mi.dwFlags = buttonDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP; input.mi.mouseData = XBUTTON1; break;
        case 5: input.mi.dwFlags = buttonDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP; input.mi.mouseData = XBUTTON2; break;
        default: return FALSE;
    }
    return submit(&input, result);
}
BOOL inputSimulationTesterMoveMouse(LONG dx, LONG dy, InputSimulationResult *result) {
    INPUT input; memset(&input, 0, sizeof(input)); input.type = INPUT_MOUSE;
    input.mi.dx = dx; input.mi.dy = dy; input.mi.dwFlags = MOUSEEVENTF_MOVE;
    return submit(&input, result);
}
BOOL inputSimulationTesterSendWheel(LONG delta, BOOL horizontal, InputSimulationResult *result) {
    INPUT input; memset(&input, 0, sizeof(input)); input.type = INPUT_MOUSE;
    input.mi.dwFlags = horizontal ? MOUSEEVENTF_HWHEEL : MOUSEEVENTF_WHEEL;
    input.mi.mouseData = (DWORD)delta;
    return submit(&input, result);
}

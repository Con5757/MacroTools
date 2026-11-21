#define WIN32_LEAN_AND_MEAN
#include "keyboardInputSimulator.h"

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

static BOOL sendVirtualKey(WORD virtualKey, BOOL keyDown, BOOL extended) {
    if (virtualKey == 0) return FALSE;
    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = (extended ? KEYEVENTF_EXTENDEDKEY : 0) |
                       (keyDown ? 0 : KEYEVENTF_KEYUP);
    return SendInput(1, &input, sizeof(input)) == 1;
}

BOOL keyboardInputSimulatorKeyDown(WORD virtualKey, BOOL extended) {
    return sendVirtualKey(virtualKey, TRUE, extended);
}

BOOL keyboardInputSimulatorKeyUp(WORD virtualKey, BOOL extended) {
    return sendVirtualKey(virtualKey, FALSE, extended);
}

BOOL keyboardInputSimulatorTapKey(WORD virtualKey, BOOL extended) {
    if (!sendVirtualKey(virtualKey, TRUE, extended)) return FALSE;
    return sendVirtualKey(virtualKey, FALSE, extended);
}

BOOL keyboardInputSimulatorSendScanCode(WORD scanCode, BOOL keyDown,
                                        BOOL extended) {
    if (scanCode == 0) return FALSE;
    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = scanCode;
    input.ki.dwFlags = KEYEVENTF_SCANCODE |
                       (extended ? KEYEVENTF_EXTENDEDKEY : 0) |
                       (keyDown ? 0 : KEYEVENTF_KEYUP);
    return SendInput(1, &input, sizeof(input)) == 1;
}

BOOL keyboardInputSimulatorSendUnicodeText(const WCHAR *text) {
    if (text == NULL) return FALSE;
    size_t length = 0;
    while (text[length] != 0) {
        ++length;
    }
    if (length == 0) return TRUE;
    if (length > (size_t)(UINT_MAX / 2) ||
        length > SIZE_MAX / (2 * sizeof(INPUT))) return FALSE;

    UINT inputCount = (UINT)(length * 2);
    INPUT *inputs = (INPUT *)calloc(inputCount, sizeof(INPUT));
    if (inputs == NULL) return FALSE;

    for (size_t i = 0; i < length; ++i) {
        inputs[i * 2].type = INPUT_KEYBOARD;
        inputs[i * 2].ki.wScan = (WORD)text[i];
        inputs[i * 2].ki.dwFlags = KEYEVENTF_UNICODE;

        inputs[i * 2 + 1].type = INPUT_KEYBOARD;
        inputs[i * 2 + 1].ki.wScan = (WORD)text[i];
        inputs[i * 2 + 1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    }

    UINT sent = SendInput(inputCount, inputs, sizeof(INPUT));
    free(inputs);
    return sent == inputCount;
}

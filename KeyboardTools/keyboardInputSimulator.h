#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stddef.h>

BOOL keyboardInputSimulatorKeyDown(WORD virtualKey, BOOL extended);
BOOL keyboardInputSimulatorKeyUp(WORD virtualKey, BOOL extended);
BOOL keyboardInputSimulatorTapKey(WORD virtualKey, BOOL extended);
BOOL keyboardInputSimulatorSendScanCode(WORD scanCode, BOOL keyDown,
                                        BOOL extended);
BOOL keyboardInputSimulatorSendUnicodeText(const WCHAR *text);
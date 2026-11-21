#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <stddef.h>

typedef struct {
  DWORD virtualKey;
  DWORD scanCode;
  DWORD flags;
  BOOL keyDown;
  ULONGLONG timestampMs;
} KeyboardRecordedEvent;

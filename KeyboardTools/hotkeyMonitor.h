#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct {
  int id;
  UINT modifiers;
  UINT virtualKey;
  BOOL isRegistered;
} HotkeyMonitor;

typedef struct {
  int id;
  UINT modifiers;
  UINT virtualKey;
  ULONGLONG timestampMs;
} HotkeyMonitorEvent;

BOOL hotkeyMonitorInitialize(HotkeyMonitor *monitor);
BOOL hotkeyMonitorRegister(HotkeyMonitor *monitor, int id, UINT modifiers,
                           UINT virtualKey);
BOOL hotkeyMonitorHandleMessage(const HotkeyMonitor *monitor,
                                const MSG *message, HotkeyMonitorEvent *event);
void hotkeyMonitorUnregister(HotkeyMonitor *monitor);

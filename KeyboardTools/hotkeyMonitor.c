#define WIN32_LEAN_AND_MEAN
#include "hotkeyMonitor.h"

BOOL hotkeyMonitorInitialize(HotkeyMonitor *monitor) {
  if (monitor == NULL)
    return FALSE;
  ZeroMemory(monitor, sizeof(*monitor));
  return TRUE;
}

BOOL hotkeyMonitorRegister(HotkeyMonitor *monitor, int id, UINT modifiers,
                           UINT virtualKey) {
  if (monitor == NULL || id == 0 || virtualKey == 0 || virtualKey > 255) {
    return FALSE;
  }
  if (monitor->isRegistered)
    hotkeyMonitorUnregister(monitor);

  if (!RegisterHotKey(NULL, id, modifiers, virtualKey))
    return FALSE;

  monitor->id = id;
  monitor->modifiers = modifiers;
  monitor->virtualKey = virtualKey;
  monitor->isRegistered = TRUE;
  return TRUE;
}

BOOL hotkeyMonitorHandleMessage(const HotkeyMonitor *monitor,
                                const MSG *message, HotkeyMonitorEvent *event) {
  if (monitor == NULL || message == NULL || event == NULL ||
      !monitor->isRegistered || message->message != WM_HOTKEY ||
      (int)message->wParam != monitor->id) {
    return FALSE;
  }

  event->id = monitor->id;
  event->modifiers = (UINT)LOWORD(message->lParam);
  event->virtualKey = (UINT)HIWORD(message->lParam);
  event->timestampMs = GetTickCount64();
  return TRUE;
}

void hotkeyMonitorUnregister(HotkeyMonitor *monitor) {
  if (monitor == NULL)
    return;
  if (monitor->isRegistered)
    UnregisterHotKey(NULL, monitor->id);
  monitor->isRegistered = FALSE;
}
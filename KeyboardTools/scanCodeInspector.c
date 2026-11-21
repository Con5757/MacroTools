#define WIN32_LEAN_AND_MEAN
#include "scanCodeInspector.h"

#define SCAN_CODE_INJECTED_FLAGS (LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED)
static ScanCodeInspector *g_activeScanInspector = NULL;

static LRESULT CALLBACK scanCodeHookProc(int code, WPARAM message,
                                         LPARAM parameter) {
  if (code == HC_ACTION && g_activeScanInspector != NULL &&
      g_activeScanInspector->isRunning) {
    BOOL keyDown = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
    BOOL keyUp = message == WM_KEYUP || message == WM_SYSKEYUP;
    if (keyDown || keyUp) {
      const KBDLLHOOKSTRUCT *key = (const KBDLLHOOKSTRUCT *)parameter;
      ScanCodeInspector *inspector = g_activeScanInspector;
      inspector->lastEvent.virtualKey = key->vkCode;
      inspector->lastEvent.scanCode = key->scanCode;
      inspector->lastEvent.flags = key->flags;
      inspector->lastEvent.message = (UINT)message;
      inspector->lastEvent.keyDown = keyDown;
      inspector->lastEvent.extended = (key->flags & LLKHF_EXTENDED) != 0;
      inspector->lastEvent.injected =
          (key->flags & SCAN_CODE_INJECTED_FLAGS) != 0;
      inspector->lastEvent.timestampMs = GetTickCount64();
      inspector->hasEvent = TRUE;
      inspector->totalEvents++;
    }
  }

  return CallNextHookEx(
      g_activeScanInspector != NULL ? g_activeScanInspector->hook : NULL, code,
      message, parameter);
}

BOOL scanCodeInspectorInitialize(ScanCodeInspector *inspector) {
  if (inspector == NULL)
    return FALSE;
  ZeroMemory(inspector, sizeof(*inspector));
  return TRUE;
}

BOOL scanCodeInspectorStart(ScanCodeInspector *inspector) {
  if (inspector == NULL ||
      (g_activeScanInspector != NULL && g_activeScanInspector != inspector))
    return FALSE;
  scanCodeInspectorStop(inspector);
  inspector->hasEvent = FALSE;
  inspector->totalEvents = 0;
  g_activeScanInspector = inspector;
  inspector->hook = SetWindowsHookExA(WH_KEYBOARD_LL, scanCodeHookProc,
                                      GetModuleHandleA(NULL), 0);
  if (inspector->hook == NULL) {
    g_activeScanInspector = NULL;
    return FALSE;
  }
  inspector->isRunning = TRUE;
  return TRUE;
}

void scanCodeInspectorStop(ScanCodeInspector *inspector) {
  if (inspector == NULL)
    return;
  inspector->isRunning = FALSE;
  if (inspector->hook != NULL) {
    UnhookWindowsHookEx(inspector->hook);
    inspector->hook = NULL;
  }
  if (g_activeScanInspector == inspector)
    g_activeScanInspector = NULL;
}

BOOL scanCodeInspectorGetLastEvent(const ScanCodeInspector *inspector,
                                   ScanCodeInspectorEvent *event,
                                   ULONGLONG *totalEvents) {
  if (inspector == NULL || event == NULL || !inspector->hasEvent)
    return FALSE;
  *event = inspector->lastEvent;
  if (totalEvents != NULL)
    *totalEvents = inspector->totalEvents;
  return TRUE;
}
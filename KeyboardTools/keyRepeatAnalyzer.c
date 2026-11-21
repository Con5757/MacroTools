#define WIN32_LEAN_AND_MEAN
#include "keyRepeatAnalyzer.h"

static KeyRepeatAnalyzer *g_activeRepeatAnalyzer = NULL;

static LRESULT CALLBACK keyRepeatHookProc(int code, WPARAM message,
                                          LPARAM parameter) {
  if (code == HC_ACTION && g_activeRepeatAnalyzer != NULL &&
      g_activeRepeatAnalyzer->isRunning) {
    const KBDLLHOOKSTRUCT *key = (const KBDLLHOOKSTRUCT *)parameter;
    if (key->vkCode < 256) {
      KeyRepeatAnalyzer *analyzer = g_activeRepeatAnalyzer;
      BOOL keyDown = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
      BOOL keyUp = message == WM_KEYUP || message == WM_SYSKEYUP;
      BOOL injected =
          (key->flags & (LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED)) != 0;

      if ((keyDown || keyUp) && (analyzer->includeInjected || !injected)) {
        unsigned int vk = (unsigned int)key->vkCode;
        ULONGLONG now = GetTickCount64();
        KeyRepeatStats *stats = &analyzer->stats[vk];

        if (keyDown) {
          stats->keyDownCount++;
          if (analyzer->keyHeld[vk]) {
            ULONGLONG interval = now - analyzer->lastKeyDownAt[vk];
            stats->repeatCount++;
            stats->lastRepeatIntervalMs = interval;
            stats->totalRepeatIntervalMs += interval;
            stats->averageRepeatIntervalMs =
                (double)stats->totalRepeatIntervalMs /
                (double)stats->repeatCount;
            if (stats->averageRepeatIntervalMs > 0.0) {
              stats->repeatsPerSecond = 1000.0 / stats->averageRepeatIntervalMs;
            }
          }
          analyzer->keyHeld[vk] = 1;
          analyzer->lastKeyDownAt[vk] = now;
        } else if (keyUp) {
          analyzer->keyHeld[vk] = 0;
        }
      }
    }
  }

  return CallNextHookEx(
      g_activeRepeatAnalyzer != NULL ? g_activeRepeatAnalyzer->hook : NULL,
      code, message, parameter);
}

BOOL keyRepeatAnalyzerInitialize(KeyRepeatAnalyzer *analyzer) {
  if (analyzer == NULL)
    return FALSE;
  ZeroMemory(analyzer, sizeof(*analyzer));
  return TRUE;
}

BOOL keyRepeatAnalyzerStart(KeyRepeatAnalyzer *analyzer, BOOL includeInjected) {
  if (analyzer == NULL ||
      (g_activeRepeatAnalyzer != NULL && g_activeRepeatAnalyzer != analyzer))
    return FALSE;
  keyRepeatAnalyzerStop(analyzer);
  ZeroMemory(analyzer->keyHeld, sizeof(analyzer->keyHeld));
  ZeroMemory(analyzer->lastKeyDownAt, sizeof(analyzer->lastKeyDownAt));
  ZeroMemory(analyzer->stats, sizeof(analyzer->stats));
  analyzer->includeInjected = includeInjected;
  g_activeRepeatAnalyzer = analyzer;
  analyzer->hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyRepeatHookProc,
                                     GetModuleHandleA(NULL), 0);
  if (analyzer->hook == NULL) {
    g_activeRepeatAnalyzer = NULL;
    return FALSE;
  }
  analyzer->isRunning = TRUE;
  return TRUE;
}

void keyRepeatAnalyzerStop(KeyRepeatAnalyzer *analyzer) {
  if (analyzer == NULL)
    return;
  analyzer->isRunning = FALSE;
  if (analyzer->hook != NULL) {
    UnhookWindowsHookEx(analyzer->hook);
    analyzer->hook = NULL;
  }
  if (g_activeRepeatAnalyzer == analyzer)
    g_activeRepeatAnalyzer = NULL;
}

BOOL keyRepeatAnalyzerGetStats(const KeyRepeatAnalyzer *analyzer,
                               WORD virtualKey, KeyRepeatStats *stats) {
  if (analyzer == NULL || stats == NULL || virtualKey > 255)
    return FALSE;
  *stats = analyzer->stats[virtualKey];
  return TRUE;
}

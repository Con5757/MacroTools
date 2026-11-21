#define WIN32_LEAN_AND_MEAN
#include "keyRecorder.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define KEY_RECORDER_INITIAL_CAPACITY 256u
static KeyRecorder *g_activeKeyRecorder = NULL;

static BOOL appendEvent(KeyRecorder *recorder, const KBDLLHOOKSTRUCT *key,
                        BOOL keyDown) {
  if (recorder->eventCount == recorder->capacity) {
    size_t nextCapacity = recorder->capacity == 0
                              ? KEY_RECORDER_INITIAL_CAPACITY
                              : recorder->capacity * 2;

    if (nextCapacity < recorder->capacity ||
        nextCapacity > SIZE_MAX / sizeof(*recorder->events)) {
      recorder->allocationFailed = TRUE;
      return FALSE;
    }

    KeyboardRecordedEvent *resized = (KeyboardRecordedEvent *)realloc(
        recorder->events, nextCapacity * sizeof(*recorder->events));
    if (resized == NULL) {
      recorder->allocationFailed = TRUE;
      return FALSE;
    }

    recorder->events = resized;
    recorder->capacity = nextCapacity;
  }

  KeyboardRecordedEvent *event = &recorder->events[recorder->eventCount++];
  event->virtualKey = key->vkCode;
  event->scanCode = key->scanCode;
  event->flags = key->flags;
  event->keyDown = keyDown;
  event->timestampMs = GetTickCount64();
  return TRUE;
}

static LRESULT CALLBACK keyRecorderHookProc(int code, WPARAM message,
                                            LPARAM parameter) {
  if (code == HC_ACTION && g_activeKeyRecorder != NULL &&
      g_activeKeyRecorder->isRecording) {
    const KBDLLHOOKSTRUCT *key = (const KBDLLHOOKSTRUCT *)parameter;
    BOOL keyDown = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
    BOOL keyUp = message == WM_KEYUP || message == WM_SYSKEYUP;

    if (keyDown || keyUp) {
      (void)appendEvent(g_activeKeyRecorder, key, keyDown);
    }
  }

  return CallNextHookEx(g_activeKeyRecorder != NULL ? g_activeKeyRecorder->hook
                                                    : NULL,
                        code, message, parameter);
}

BOOL keyRecorderInitialize(KeyRecorder *recorder) {
  if (recorder == NULL) {
    return FALSE;
  }
  ZeroMemory(recorder, sizeof(*recorder));
  return TRUE;
}

BOOL keyRecorderStart(KeyRecorder *recorder) {
  if (recorder == NULL ||
      (g_activeKeyRecorder != NULL && g_activeKeyRecorder != recorder)) {
    return FALSE;
  }

  keyRecorderStop(recorder);
  keyRecorderClear(recorder);
  recorder->allocationFailed = FALSE;
  g_activeKeyRecorder = recorder;
  recorder->hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyRecorderHookProc,
                                     GetModuleHandleA(NULL), 0);
  if (recorder->hook == NULL) {
    g_activeKeyRecorder = NULL;
    return FALSE;
  }

  recorder->isRecording = TRUE;
  return TRUE;
}

void keyRecorderStop(KeyRecorder *recorder) {
  if (recorder == NULL) {
    return;
  }

  recorder->isRecording = FALSE;
  if (recorder->hook != NULL) {
    UnhookWindowsHookEx(recorder->hook);
    recorder->hook = NULL;
  }
  if (g_activeKeyRecorder == recorder) {
    g_activeKeyRecorder = NULL;
  }
}

void keyRecorderClear(KeyRecorder *recorder) {
  if (recorder != NULL) {
    recorder->eventCount = 0;
    recorder->allocationFailed = FALSE;
  }
}

void keyRecorderDestroy(KeyRecorder *recorder) {
  if (recorder == NULL) {
    return;
  }
  keyRecorderStop(recorder);
  free(recorder->events);
  ZeroMemory(recorder, sizeof(*recorder));
}

const KeyboardRecordedEvent *keyRecorderGetEvents(const KeyRecorder *recorder,
                                                  size_t *eventCount) {
  if (eventCount != NULL) {
    *eventCount = recorder != NULL ? recorder->eventCount : 0;
  }
  return recorder != NULL ? recorder->events : NULL;
}

BOOL keyRecorderIsRecording(const KeyRecorder *recorder) {
  return recorder != NULL && recorder->isRecording;
}

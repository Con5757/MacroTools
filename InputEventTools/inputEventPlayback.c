#include "inputEventPlayback.h"
#include <float.h>
#include <string.h>

/*
returns 1 when sent
returns 0 for nonplayable
returns -1 on failure
*/
static int sendOne(const InputEvent *event) {
  INPUT input;
  memset(&input, 0, sizeof(input));
  input.type = INPUT_MOUSE;
  switch (event->type) {
  case INPUT_EVENT_KEY_DOWN:
  case INPUT_EVENT_KEY_UP:
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = (WORD)event->virtualKey;
    input.ki.wScan = (WORD)event->scanCode;
    input.ki.dwFlags = event->scanCode != 0 ? KEYEVENTF_SCANCODE : 0;
    if (event->type == INPUT_EVENT_KEY_UP)
      input.ki.dwFlags |= KEYEVENTF_KEYUP;
    if ((event->rawKeyboardFlags & RI_KEY_E0) != 0)
      input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    if (event->virtualKey == 0 && event->scanCode == 0)
      return -1;
    break;
  case INPUT_EVENT_MOUSE_MOVE:
    input.type = INPUT_MOUSE;
    input.mi.dx = event->x;
    input.mi.dy = event->y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    if ((event->rawMouseFlags & MOUSE_MOVE_ABSOLUTE) != 0) {
      input.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    }
    break;
  case INPUT_EVENT_MOUSE_BUTTON_DOWN:
  case INPUT_EVENT_MOUSE_BUTTON_UP: {
    BOOL down = event->type == INPUT_EVENT_MOUSE_BUTTON_DOWN;
    input.type = INPUT_MOUSE;
    switch (event->mouseButton) {
    case 1:
      input.mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
      break;
    case 2:
      input.mi.dwFlags = down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
      break;
    case 3:
      input.mi.dwFlags = down ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
      break;
    case 4:
      input.mi.dwFlags = down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
      input.mi.mouseData = XBUTTON1;
      break;
    case 5:
      input.mi.dwFlags = down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
      input.mi.mouseData = XBUTTON2;
      break;
    default:
      return -1;
    }
    break;
  }
  case INPUT_EVENT_MOUSE_WHEEL:
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = (event->rawButtonFlags & RI_MOUSE_HWHEEL)
                           ? MOUSEEVENTF_HWHEEL
                           : MOUSEEVENTF_WHEEL;
    input.mi.mouseData = (DWORD)event->wheelDelta;
    break;
  default:
    return 0; /* Device notifications are not replayable. */
  }
  return SendInput(1, &input, sizeof(input)) == 1 ? 1 : -1;
}
BOOL inputEventPlaybackStart(InputEventPlayback *playback,
                             const InputEvent *events, size_t count,
                             double speed) {
  if (playback == NULL || (count != 0 && events == NULL) || speed <= 0.0 ||
      speed != speed || speed > DBL_MAX)
    return FALSE;
  memset(playback, 0, sizeof(*playback));
  playback->events = events;
  playback->count = count;
  playback->speed = speed;
  playback->startTimeMs = GetTickCount64();
  playback->firstEventTimeMs = count == 0 ? 0 : events[0].timestampMs;
  playback->active = count != 0;
  return TRUE;
}
UINT inputEventPlaybackUpdate(InputEventPlayback *playback) {
  if (playback == NULL || !playback->active || playback->failed)
    return 0;
  ULONGLONG elapsed = GetTickCount64() - playback->startTimeMs;
  UINT sentThisCall = 0;
  while (playback->index < playback->count) {
    const InputEvent *event = &playback->events[playback->index];
    ULONGLONG relative = event->timestampMs >= playback->firstEventTimeMs
                             ? event->timestampMs - playback->firstEventTimeMs
                             : 0;
    ULONGLONG due = (ULONGLONG)((double)relative / playback->speed);
    if (elapsed < due)
      break;
    int sent = sendOne(event);
    if (sent < 0) {
      playback->failed = TRUE;
      playback->active = FALSE;
      break;
    }
    ++playback->index;
    if (sent > 0) {
      ++sentThisCall;
      ++playback->sentCount;
    }
  }
  if (playback->index >= playback->count)
    playback->active = FALSE;
  return sentThisCall;
}
void inputEventPlaybackStop(InputEventPlayback *playback) {
  if (playback != NULL)
    playback->active = FALSE;
}
BOOL inputEventPlaybackIsFinished(const InputEventPlayback *playback) {
  return playback == NULL ||
         (!playback->active &&
          (playback->index >= playback->count || playback->failed));
}
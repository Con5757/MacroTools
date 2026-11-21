#define WIN32_LEAN_AND_MEAN
#include "keyPlayback.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static BOOL sendRecordedEvent(const KeyboardRecordedEvent *event) {
  INPUT input;
  ZeroMemory(&input, sizeof(input));
  input.type = INPUT_KEYBOARD;

  DWORD flags = 0;
  if ((event->flags & LLKHF_EXTENDED) != 0) {
    flags |= KEYEVENTF_EXTENDEDKEY;
  }
  if (!event->keyDown) {
    flags |= KEYEVENTF_KEYUP;
  }

  if (event->scanCode != 0) {
    input.ki.wScan = (WORD)event->scanCode;
    input.ki.dwFlags = flags | KEYEVENTF_SCANCODE;
  } else {
    input.ki.wVk = (WORD)event->virtualKey;
    input.ki.dwFlags = flags;
  }

  return SendInput(1, &input, sizeof(input)) == 1;
}

static void releaseHeldKeys(KeyPlayback *playback) {
  for (unsigned int key = 0; key < 256; ++key) {
    if (playback->heldKeys[key]) {
      KeyboardRecordedEvent release = playback->heldEvents[key];
      release.keyDown = FALSE;
      (void)sendRecordedEvent(&release);
      playback->heldKeys[key] = 0;
    }
  }
}

BOOL keyPlaybackInitialize(KeyPlayback *playback) {
  if (playback == NULL)
    return FALSE;
  ZeroMemory(playback, sizeof(*playback));
  return TRUE;
}

BOOL keyPlaybackLoadEvents(KeyPlayback *playback,
                           const KeyboardRecordedEvent *events,
                           size_t eventCount) {
  if (playback == NULL || (eventCount > 0 && events == NULL) ||
      eventCount > SIZE_MAX / sizeof(*events)) {
    return FALSE;
  }

  KeyboardRecordedEvent *copy = NULL;
  if (eventCount > 0) {
    copy = (KeyboardRecordedEvent *)malloc(eventCount * sizeof(*copy));
    if (copy == NULL)
      return FALSE;
    for (size_t i = 0; i < eventCount; ++i) {
      if (i > 0 && events[i].timestampMs < events[i - 1].timestampMs) {
        free(copy);
        return FALSE;
      }
      copy[i] = events[i];
    }
  }

  keyPlaybackStop(playback);
  free(playback->events);
  playback->events = copy;
  playback->eventCount = eventCount;
  playback->nextEvent = 0;
  playback->speedMultiplier = 1.0;
  return TRUE;
}

BOOL keyPlaybackStart(KeyPlayback *playback, double speedMultiplier) {
  if (playback == NULL || playback->events == NULL ||
      playback->eventCount == 0 || !(speedMultiplier >= 0.01) ||
      !(speedMultiplier <= 100.0) || speedMultiplier != speedMultiplier) {
    return FALSE;
  }

  releaseHeldKeys(playback);
  playback->nextEvent = 0;
  playback->startTimeMs = GetTickCount64();
  playback->speedMultiplier = speedMultiplier;
  playback->isPlaying = TRUE;
  return TRUE;
}

KeyPlaybackUpdateResult keyPlaybackUpdate(KeyPlayback *playback) {
  if (playback == NULL)
    return KEY_PLAYBACK_ERROR;
  if (!playback->isPlaying)
    return KEY_PLAYBACK_IDLE;

  ULONGLONG elapsed = GetTickCount64() - playback->startTimeMs;
  BOOL sentAny = FALSE;
  ULONGLONG firstTimestamp = playback->events[0].timestampMs;

  while (playback->nextEvent < playback->eventCount) {
    const KeyboardRecordedEvent *event = &playback->events[playback->nextEvent];
    ULONGLONG relative = event->timestampMs - firstTimestamp;
    ULONGLONG dueTime =
        (ULONGLONG)((double)relative / playback->speedMultiplier);
    if (elapsed < dueTime)
      break;

    if (!sendRecordedEvent(event)) {
      keyPlaybackStop(playback);
      return KEY_PLAYBACK_ERROR;
    }

    if (event->virtualKey < 256) {
      playback->heldKeys[event->virtualKey] = event->keyDown ? 1 : 0;
      if (event->keyDown) {
        playback->heldEvents[event->virtualKey] = *event;
      }
    }
    playback->nextEvent++;
    sentAny = TRUE;
  }

  if (playback->nextEvent >= playback->eventCount) {
    releaseHeldKeys(playback);
    playback->isPlaying = FALSE;
    return KEY_PLAYBACK_FINISHED;
  }

  return sentAny ? KEY_PLAYBACK_SENT_INPUT : KEY_PLAYBACK_IDLE;
}

void keyPlaybackStop(KeyPlayback *playback) {
  if (playback == NULL)
    return;
  releaseHeldKeys(playback);
  playback->isPlaying = FALSE;
}

void keyPlaybackDestroy(KeyPlayback *playback) {
  if (playback == NULL)
    return;
  keyPlaybackStop(playback);
  free(playback->events);
  ZeroMemory(playback, sizeof(*playback));
}

BOOL keyPlaybackIsPlaying(const KeyPlayback *playback) {
  return playback != NULL && playback->isPlaying;
}
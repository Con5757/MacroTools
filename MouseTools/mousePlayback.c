#define WIN32_LEAN_AND_MEAN
#include "mousePlayback.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

BOOL mousePlaybackInitialize(MousePlayback *playback) {
  if (playback == NULL) {
    return FALSE;
  }

  ZeroMemory(playback, sizeof(*playback));
  return TRUE;
}

BOOL mousePlaybackLoadSamples(MousePlayback *playback,
                              const MouseRecorderSample *samples,
                              size_t sampleCount) {
  if (playback == NULL || (sampleCount > 0 && samples == NULL) ||
      sampleCount > SIZE_MAX / sizeof(*samples)) {
    return FALSE;
  }

  MouseRecorderSample *copy = NULL;

  if (sampleCount > 0) {
    copy = (MouseRecorderSample *)malloc(sampleCount * sizeof(*copy));
    if (copy == NULL) {
      return FALSE;
    }

    for (size_t i = 0; i < sampleCount; ++i) {
      if (i > 0 && samples[i].timestampMs < samples[i - 1].timestampMs) {
        free(copy);
        return FALSE;
      }
      copy[i] = samples[i];
    }
  }

  free(playback->samples);
  playback->samples = copy;
  playback->sampleCount = sampleCount;
  playback->nextSample = 0;
  playback->startTimeMs = 0;
  playback->cycleDurationMs =
      sampleCount > 0 ? (samples[sampleCount - 1].timestampMs == UINT64_MAX
                             ? UINT64_MAX
                             : samples[sampleCount - 1].timestampMs + 1)
                      : 1;
  playback->isPlaying = FALSE;
  playback->loop = FALSE;

  return TRUE;
}

BOOL mousePlaybackStart(MousePlayback *playback, BOOL loop) {
  if (playback == NULL || playback->samples == NULL ||
      playback->sampleCount == 0 || playback->cycleDurationMs == 0) {
    return FALSE;
  }

  playback->nextSample = 0;
  playback->startTimeMs = GetTickCount64();
  playback->loop = loop;
  playback->isPlaying = TRUE;

  return TRUE;
}

MousePlaybackUpdateResult mousePlaybackUpdate(MousePlayback *playback) {
  if (playback == NULL) {
    return MOUSE_PLAYBACK_ERROR;
  }

  if (!playback->isPlaying) {
    return MOUSE_PLAYBACK_IDLE;
  }

  ULONGLONG now = GetTickCount64();
  ULONGLONG elapsed = now - playback->startTimeMs;
  BOOL moved = FALSE;

  for (;;) {
    if (playback->nextSample >= playback->sampleCount) {
      if (!playback->loop) {
        playback->isPlaying = FALSE;
        return moved ? MOUSE_PLAYBACK_MOVED : MOUSE_PLAYBACK_FINISHED;
      }

      if (elapsed < playback->cycleDurationMs) {
        return moved ? MOUSE_PLAYBACK_MOVED : MOUSE_PLAYBACK_IDLE;
      }

      playback->startTimeMs += playback->cycleDurationMs;
      elapsed -= playback->cycleDurationMs;
      playback->nextSample = 0;
    }

    const MouseRecorderSample *sample =
        &playback->samples[playback->nextSample];

    if (sample->timestampMs > elapsed) {
      return moved ? MOUSE_PLAYBACK_MOVED : MOUSE_PLAYBACK_IDLE;
    }

    if (!SetCursorPos(sample->position.x, sample->position.y)) {
      playback->isPlaying = FALSE;
      return MOUSE_PLAYBACK_ERROR;
    }

    playback->nextSample++;
    moved = TRUE;
  }
}

void mousePlaybackStop(MousePlayback *playback) {
  if (playback != NULL) {
    playback->isPlaying = FALSE;
  }
}

void mousePlaybackDestroy(MousePlayback *playback) {
  if (playback == NULL) {
    return;
  }

  free(playback->samples);
  ZeroMemory(playback, sizeof(*playback));
}
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "mouseRecorder.h"
#include <Windows.h>
#include <stddef.h>

typedef enum {
  MOUSE_PLAYBACK_ERROR = -1,
  MOUSE_PLAYBACK_IDLE = 0,
  MOUSE_PLAYBACK_MOVED = 1,
  MOUSE_PLAYBACK_FINISHED = 2
} MousePlaybackUpdateResult;

typedef struct {
  MouseRecorderSample *samples;
  size_t sampleCount;
  size_t nextSample;
  ULONGLONG startTimeMs;
  ULONGLONG cycleDurationMs;
  BOOL isPlaying;
  BOOL loop;
} MousePlayback;

BOOL mousePlaybackInitialize(MousePlayback *playback);
BOOL mousePlaybackLoadSamples(MousePlayback *playback,
                              const MouseRecorderSample *samples,
                              size_t sampleCount);
BOOL mousePlaybackStart(MousePlayback *playback, BOOL loop);
MousePlaybackUpdateResult mousePlaybackUpdate(MousePlayback *playback);
void mousePlaybackStop(MousePlayback *playback);
void mousePlaybackDestroy(MousePlayback *playback);

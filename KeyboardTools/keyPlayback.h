#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stddef.h>
#include "keyboardToolsCommon.h"

typedef enum {
    KEY_PLAYBACK_ERROR = -1,
    KEY_PLAYBACK_IDLE = 0,
    KEY_PLAYBACK_SENT_INPUT = 1,
    KEY_PLAYBACK_FINISHED = 2
} KeyPlaybackUpdateResult;

typedef struct {
    KeyboardRecordedEvent *events;
    size_t eventCount;
    size_t nextEvent;
    ULONGLONG startTimeMs;
    double speedMultiplier;
    BYTE heldKeys[256];
    KeyboardRecordedEvent heldEvents[256];
    BOOL isPlaying;
} KeyPlayback;

BOOL keyPlaybackInitialize(KeyPlayback *playback);
BOOL keyPlaybackLoadEvents(KeyPlayback *playback,
                           const KeyboardRecordedEvent *events,
                           size_t eventCount);
BOOL keyPlaybackStart(KeyPlayback *playback, double speedMultiplier);
KeyPlaybackUpdateResult keyPlaybackUpdate(KeyPlayback *playback);
void keyPlaybackStop(KeyPlayback *playback);
void keyPlaybackDestroy(KeyPlayback *playback);
BOOL keyPlaybackIsPlaying(const KeyPlayback *playback);

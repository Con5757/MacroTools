#pragma once
#include "inputEventCommon.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct InputEventPlayback {
    const InputEvent *events;
    size_t count;
    size_t index;
    double speed;
    ULONGLONG startTimeMs;
    ULONGLONG firstEventTimeMs;
    BOOL active;
    BOOL failed;
    UINT sentCount;
} InputEventPlayback;
BOOL inputEventPlaybackStart(InputEventPlayback *playback, const InputEvent *events,
                             size_t count, double speed);
UINT inputEventPlaybackUpdate(InputEventPlayback *playback); // returns sent in one call
void inputEventPlaybackStop(InputEventPlayback *playback);
BOOL inputEventPlaybackIsFinished(const InputEventPlayback *playback);
#ifdef __cplusplus
}
#endif
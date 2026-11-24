#include "macroRecorder.h"
#include <string.h>

void macroRecorderInitialize(MacroRecorder *recorder) {
  if (recorder != NULL)
    memset(recorder, 0, sizeof(*recorder));
}
bool macroRecorderStart(MacroRecorder *recorder, MacroSequence *sequence,
                        uint64_t now_ms, bool clear_existing) {
  if (recorder == NULL || sequence == NULL || recorder->recording)
    return false;
  if (clear_existing)
    sequence->count = 0;
  recorder->sequence = sequence;
  recorder->last_event_ms = now_ms;
  recorder->has_previous = true;
  recorder->recording = true;
  return true;
}
bool macroRecorderRecord(MacroRecorder *recorder, const MacroStep *event,
                         uint64_t event_time_ms) {
  if (recorder == NULL || event == NULL || !recorder->recording ||
      recorder->sequence == NULL)
    return false;
  MacroStep step = *event;
  step.delay_ms =
      recorder->has_previous && event_time_ms >= recorder->last_event_ms
          ? event_time_ms - recorder->last_event_ms
          : 0;
  step.id = 0;
  if (!macroSequenceAppend(recorder->sequence, &step))
    return false;
  if (!recorder->has_previous || event_time_ms > recorder->last_event_ms) {
    recorder->last_event_ms = event_time_ms;
  }
  recorder->has_previous = true;
  return true;
}
bool macroRecorderStop(MacroRecorder *recorder) {
  if (recorder == NULL || !recorder->recording)
    return false;
  recorder->recording = false;
  recorder->sequence = NULL;
  recorder->has_previous = false;
  return true;
}
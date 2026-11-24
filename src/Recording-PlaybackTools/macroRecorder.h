#pragma once
#include "macroToolsCommon.h"

typedef struct {
  MacroSequence *sequence;
  uint64_t last_event_ms;
  bool recording;
  bool has_previous;
} MacroRecorder;

void macroRecorderInitialize(MacroRecorder *recorder);
bool macroRecorderStart(MacroRecorder *recorder, MacroSequence *sequence,
                        uint64_t now_ms, bool clear_existing);
bool macroRecorderRecord(MacroRecorder *recorder, const MacroStep *event,
                         uint64_t event_time_ms);
bool macroRecorderStop(MacroRecorder *recorder);
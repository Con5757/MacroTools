#pragma once
#include "macroToolsCommon.h"

typedef struct {
  const MacroSequence *sequence;
  uint64_t scheduled_at_ms;
  bool armed;
} MacroSchedule;

void macroSchedulerInitialize(MacroSchedule *schedule);
bool macroSchedulerArm(MacroSchedule *schedule, const MacroSequence *sequence,
                       uint64_t scheduled_at_ms);
bool macroSchedulerPoll(MacroSchedule *schedule, uint64_t now_ms,
                        const MacroSequence **due_sequence);
void macroSchedulerCancel(MacroSchedule *schedule);
#include "macroScheduler.h"
#include <string.h>

void macroSchedulerInitialize(MacroSchedule *schedule) {
  if (schedule != NULL) {
    memset(schedule, 0, sizeof(*schedule));
  }
}
bool macroSchedulerArm(MacroSchedule *schedule, const MacroSequence *sequence,
                       uint64_t scheduled_at_ms) {
  if (schedule == NULL || sequence == NULL || sequence->count == 0) {
    return false;
  }
  schedule->sequence = sequence;
  schedule->scheduled_at_ms = scheduled_at_ms;
  schedule->armed = true;
  return true;
}
bool macroSchedulerPoll(MacroSchedule *schedule, uint64_t now_ms,
                        const MacroSequence **due_sequence) {
  if (due_sequence != NULL) {
    *due_sequence = NULL;
  }
  if (schedule == NULL || !schedule->armed ||
      now_ms < schedule->scheduled_at_ms) {
    return false;
  }
  if (due_sequence != NULL) {
    *due_sequence = schedule->sequence;
  }
  schedule->armed = false;
  return true;
}
void macroSchedulerCancel(MacroSchedule *schedule) {
  if (schedule != NULL) {
    memset(schedule, 0, sizeof(*schedule));
  }
}
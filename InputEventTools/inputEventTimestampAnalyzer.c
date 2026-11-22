#include "inputEventTimestampAnalyzer.h"
#include <math.h>
#include <string.h>

BOOL inputEventTimestampAnalyzerAnalyze(const InputEvent *events, size_t count,
                                        InputEventTimestampStats *stats) {
  if (stats == NULL || (count && events == NULL))
    return FALSE;
  memset(stats, 0, sizeof(*stats));
  stats->eventCount = count;
  if (count < 2)
    return TRUE;
  stats->minimumIntervalMs = ~(ULONGLONG)0;
  stats->durationMs =
      events[count - 1].timestampMs >= events[0].timestampMs
          ? events[count - 1].timestampMs - events[0].timestampMs
          : 0;
  double mean = 0.0, m2 = 0.0;
  for (size_t i = 1; i < count; ++i) {
    if (events[i].timestampMs < events[i - 1].timestampMs) {
      ++stats->outOfOrderCount;
      continue;
    }
    ULONGLONG interval = events[i].timestampMs - events[i - 1].timestampMs;
    if (interval < stats->minimumIntervalMs)
      stats->minimumIntervalMs = interval;
    if (interval > stats->maximumIntervalMs)
      stats->maximumIntervalMs = interval;
    ++stats->intervalCount;
    double delta = (double)interval - mean;
    mean += delta / (double)stats->intervalCount;
    m2 += delta * ((double)interval - mean);
  }
  if (stats->intervalCount != 0) {
    stats->meanIntervalMs = mean;
    stats->standardDeviationMs = sqrt(m2 / (double)stats->intervalCount);
  } else {
    stats->minimumIntervalMs = 0;
  }
  return TRUE;
}
#include "debounceUtility.h"

void laDebounceInitialize(LADebouncer *d, uint64_t interval) {
  if (!d)
    return;
  d->intervalMs = interval;
  d->lastAcceptedMs = 0;
  d->initialized = false;
}
bool laDebounceAccept(LADebouncer *d, uint64_t now) {
  if (!d)
    return false;
  if (!d->initialized || now < d->lastAcceptedMs ||
      now - d->lastAcceptedMs >= d->intervalMs) {
    d->lastAcceptedMs = now;
    d->initialized = true;
    return true;
  }
  return false;
}
void laDebounceReset(LADebouncer *d) {
  if (d) {
    d->lastAcceptedMs = 0;
    d->initialized = false;
  }
}
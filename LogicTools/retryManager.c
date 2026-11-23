#include "retryManager.h"

#include <string.h>

bool laRetryBegin(LARetry *retry, uint32_t maxAttempts, uint64_t delayMs,
                  uint64_t nowMs) {
  if (retry == NULL || maxAttempts == 0)
    return false;
  retry->maxAttempts = maxAttempts;
  retry->attempts = 0;
  retry->delayMs = delayMs;
  retry->nextAttemptMs = nowMs;
  retry->status = LA_RETRY_READY;
  return true;
}

bool laRetryCanAttempt(LARetry *retry, uint64_t nowMs) {
  if (retry == NULL)
    return false;
  if (retry->status == LA_RETRY_WAITING && nowMs >= retry->nextAttemptMs) {
    retry->status = LA_RETRY_READY;
  }
  if (retry->status != LA_RETRY_READY || retry->attempts >= retry->maxAttempts)
    return false;
  retry->attempts++;
  retry->status = LA_RETRY_IN_PROGRESS;
  return true;
}

void laRetryRecordResult(LARetry *retry, bool succeeded, uint64_t nowMs) {
  if (retry == NULL || retry->status != LA_RETRY_IN_PROGRESS)
    return;
  if (succeeded) {
    retry->status = LA_RETRY_SUCCEEDED;
  } else if (retry->attempts >= retry->maxAttempts) {
    retry->status = LA_RETRY_EXHAUSTED;
  } else {
    retry->nextAttemptMs = laAddMilliseconds(nowMs, retry->delayMs);
    retry->status = LA_RETRY_WAITING;
  }
}

void laRetryReset(LARetry *retry) {
  if (retry != NULL)
    memset(retry, 0, sizeof(*retry));
}

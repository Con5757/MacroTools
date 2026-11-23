#pragma once
#include "logicToolsCommon.h"

typedef enum {
  LA_RETRY_IDLE,
  LA_RETRY_READY,
  LA_RETRY_IN_PROGRESS,
  LA_RETRY_WAITING,
  LA_RETRY_SUCCEEDED,
  LA_RETRY_EXHAUSTED
} LARetryStatus;
typedef struct {
  uint32_t maxAttempts;
  uint32_t attempts;
  uint64_t delayMs;
  uint64_t nextAttemptMs;
  LARetryStatus status;
} LARetry;

bool laRetryBegin(LARetry *retry, uint32_t maxAttempts, uint64_t delayMs,
                  uint64_t nowMs);
bool laRetryCanAttempt(LARetry *retry, uint64_t nowMs);
// must be called once after each step
void laRetryRecordResult(LARetry *retry, bool succeeded, uint64_t nowMs);
void laRetryReset(LARetry *retry);

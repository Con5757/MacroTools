#pragma once
#include "logicToolsCommon.h"

typedef void (*LATaskCallback)(void *context, uint32_t taskId, uint64_t nowMs);
typedef struct {
  uint32_t id;
  uint64_t nextRunMs;
  uint64_t intervalMs;
  LATaskCallback callback;
  void *context;
  bool repeat;
  bool occupied;
} LATask;
typedef struct {
  LATask tasks[LA_MAX_ITEMS];
} LATaskScheduler;

void laTaskSchedulerInitialize(LATaskScheduler *scheduler);
bool laTaskSchedule(LATaskScheduler *scheduler, uint32_t id,
                    uint64_t firstRunMs, uint64_t intervalMs, bool repeat,
                    LATaskCallback callback, void *context);
bool laTaskCancel(LATaskScheduler *scheduler, uint32_t id);
size_t laTaskSchedulerUpdate(LATaskScheduler *scheduler, uint64_t nowMs,
                             size_t maxTasks);
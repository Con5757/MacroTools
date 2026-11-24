#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputEventLogger {
  InputEventBuffer buffer;
} InputEventLogger;

BOOL inputEventLoggerInitialize(InputEventLogger *logger,
                                size_t initialCapacity);
BOOL inputEventLoggerRecord(InputEventLogger *logger, const InputEvent *event);
void inputEventLoggerClear(InputEventLogger *logger);
BOOL inputEventLoggerWriteCsv(const InputEventLogger *logger,
                              const char *filePath);
void inputEventLoggerDestroy(InputEventLogger *logger);

#ifdef __cplusplus
}
#endif
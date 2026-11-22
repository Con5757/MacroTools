#include "inputEventLogger.h"
#include <stdio.h>
#include <string.h>

BOOL inputEventLoggerInitialize(InputEventLogger *logger,
                                size_t initialCapacity) {
  if (logger == NULL)
    return FALSE;
  return inputEventBufferInitialize(&logger->buffer, initialCapacity);
}
BOOL inputEventLoggerRecord(InputEventLogger *logger, const InputEvent *event) {
  return logger != NULL && inputEventBufferAppend(&logger->buffer, event);
}
void inputEventLoggerClear(InputEventLogger *logger) {
  if (logger != NULL)
    inputEventBufferClear(&logger->buffer);
}
BOOL inputEventLoggerWriteCsv(const InputEventLogger *logger,
                              const char *filePath) {
  if (logger == NULL || filePath == NULL || filePath[0] == '\0')
    return FALSE;
  FILE *file = fopen(filePath, "wb");
  if (file == NULL)
    return FALSE;
  BOOL ok =
      fprintf(file,
              "timestamp_ms,message_time,type,source,device,virtual_key,scan_"
              "code,mouse_button,x,y,wheel_delta,raw_keyboard_flags,raw_mouse_"
              "flags,raw_button_flags,raw_button_data,extra_info\r\n") >= 0;
  for (size_t i = 0; ok && i < logger->buffer.count; ++i) {
    const InputEvent *e = &logger->buffer.events[i];
    ok = fprintf(
             file, "%llu,%lu,%s,%s,%p,%u,%u,%u,%ld,%ld,%ld,%u,%u,%u,%u,%lu\r\n",
             (unsigned long long)e->timestampMs, (unsigned long)e->messageTime,
             inputEventTypeName(e->type), inputEventSourceName(e->source),
             (void *)e->deviceHandle, e->virtualKey, e->scanCode,
             e->mouseButton, e->x, e->y, e->wheelDelta, e->rawKeyboardFlags,
             e->rawMouseFlags, e->rawButtonFlags, e->rawButtonData,
             (unsigned long)e->extraInfo) >= 0;
  }
  if (fclose(file) != 0)
    ok = FALSE;
  return ok;
}
void inputEventLoggerDestroy(InputEventLogger *logger) {
  if (logger != NULL)
    inputEventBufferDestroy(&logger->buffer);
}
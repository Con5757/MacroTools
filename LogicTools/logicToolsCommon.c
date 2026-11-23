#include "logicToolsCommon.h"
#include <string.h>

bool laCopyName(char destination[LA_NAME_CAPACITY], const char *source) {
  if (destination == NULL || source == NULL || source[0] == '\0') {
    return false;
  }

  size_t length = strlen(source);
  if (length >= LA_NAME_CAPACITY) {
    return false;
  }

  memcpy(destination, source, length + 1);
  return true;
}
uint64_t laAddMilliseconds(uint64_t startMs, uint64_t durationMs) {
  if (UINT64_MAX - startMs < durationMs) {
    return UINT64_MAX;
  }

  return startMs + durationMs;
}
#include "gameTimerReader.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

bool gameTimerReaderParse(const char *text, GTGameTimerValue *timer) {
  if (text == NULL || timer == NULL)
    return false;
  while (isspace((unsigned char)*text))
    ++text;
  char buffer[64];
  size_t length = strlen(text);
  while (length > 0 && isspace((unsigned char)text[length - 1]))
    --length;
  if (length == 0 || length >= sizeof(buffer))
    return false;
  memcpy(buffer, text, length);
  buffer[length] = '\0';

  uint32_t parts[3] = {0, 0, 0};
  size_t count = 0;
  char *cursor = buffer;
  while (*cursor != '\0') {
    if (count >= 3 || !isdigit((unsigned char)*cursor))
      return false;
    errno = 0;
    char *end = NULL;
    unsigned long value = strtoul(cursor, &end, 10);
    if (errno != 0 || end == cursor || value > UINT32_MAX)
      return false;
    parts[count++] = (uint32_t)value;
    if (*end == '\0') {
      cursor = end;
      break;
    }
    if (*end != ':')
      return false;
    cursor = end + 1;
    if (*cursor == '\0')
      return false;
  }
  if (count != 2 && count != 3)
    return false;

  uint32_t hours = count == 3 ? parts[0] : 0;
  uint32_t minutes = count == 3 ? parts[1] : parts[0];
  uint32_t seconds = count == 3 ? parts[2] : parts[1];
  if ((count == 3 && minutes >= 60) || seconds >= 60 ||
      hours > (UINT32_MAX / 3600u))
    return false;
  uint64_t total = (uint64_t)hours * 3600u + (uint64_t)minutes * 60u + seconds;
  if (total > UINT32_MAX)
    return false;
  timer->hours = hours;
  timer->minutes = minutes;
  timer->seconds = seconds;
  timer->totalSeconds = (uint32_t)total;
  return true;
}
bool gameTimerReaderRead(const GTImage *frame, GTRect region,
                         GTTextRecognitionCallback recognize, void *userData,
                         GTGameTimerValue *timer, char *recognizedText,
                         size_t textCapacity) {
  if (timer == NULL || recognizedText == NULL || textCapacity == 0)
    return false;
  if (!gameTextReaderRead(frame, region, recognize, userData, recognizedText,
                          textCapacity))
    return false;
  return gameTimerReaderParse(recognizedText, timer);
}
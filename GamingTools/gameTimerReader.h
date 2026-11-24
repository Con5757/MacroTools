#pragma once

#include "gameTextReader.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t hours;
  uint32_t minutes;
  uint32_t seconds;
  uint32_t totalSeconds;
} GTGameTimerValue;

bool gameTimerReaderRead(const GTImage *frame, GTRect region,
                         GTTextRecognitionCallback recognize, void *userData,
                         GTGameTimerValue *timer, char *recognizedText,
                         size_t textCapacity); // NEEDS OCR ADAPTER
bool gameTimerReaderParse(const char *text, GTGameTimerValue *timer);
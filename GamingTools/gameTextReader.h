#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stddef.h>

typedef bool (*GTTextRecognitionCallback)(const GTImage *frame, GTRect region,
                                          char *output, size_t outputCapacity,
                                          void *userData);

bool gameTextReaderRead(const GTImage *frame, GTRect region,
                        GTTextRecognitionCallback recognize, void *userData,
                        char *output,
                        size_t outputCapacity); // NEEDS CUSTOM OCR SUPPLY, WILL
                                                // BE ADDED IN RELEASES

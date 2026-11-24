#include "gameTextReader.h"

bool gameTextReaderRead(const GTImage *frame, GTRect region,
                        GTTextRecognitionCallback recognize, void *userData,
                        char *output, size_t outputCapacity) {
  if (!gtRectIsValidForImage(frame, region) || recognize == NULL ||
      output == NULL || outputCapacity == 0)
    return false;
  output[0] = '\0';
  if (!recognize(frame, region, output, outputCapacity, userData)) {
    output[0] = '\0';
    return false;
  }
  output[outputCapacity - 1u] = '\0';
  return true;
}
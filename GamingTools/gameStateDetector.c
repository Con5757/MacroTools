#include "gameStateDetector.h"
#include <float.h>

bool gameStateDetectorIdentify(const GTImage *frame,
                               const GTGameStateReference *references,
                               size_t referenceCount,
                               GTGameStateResult *result) {
  if (!gtImageIsValid(frame) || references == NULL || referenceCount == 0 ||
      result == NULL)
    return false;

  result->referenceIndex = (size_t)-1;
  result->stateName = NULL;
  result->difference = DBL_MAX;
  for (size_t i = 0; i < referenceCount; ++i) {
    GTImageMatch match;
    if (references[i].name == NULL ||
        !gtImageFindTemplate(frame, &references[i].reference,
                             references[i].maximumMeanDifference, &match)) {
      continue;
    }
    if (match.found && match.meanChannelDifference < result->difference) {
      result->referenceIndex = i;
      result->stateName = references[i].name;
      result->difference = match.meanChannelDifference;
    }
  }
  return result->stateName != NULL;
}
#include "gameMenuDetector.h"

bool gameMenuDetectorIsVisible(const GTImage *frame,
                               const GTImage *menuTemplate,
                               double maximumMeanDifference,
                               GTImageMatch *match) {
  if (!gtImageFindTemplate(frame, menuTemplate, maximumMeanDifference, match))
    return false;
  return match->found;
}
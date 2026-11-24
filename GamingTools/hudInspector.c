#include "hudInspector.h"

bool hudInspectorInspect(const GTImage *frame, GTRect hudRegion,
                         GTImageRegionStats *stats) {
  return gtImageMeasureRegion(frame, hudRegion, stats);
}   
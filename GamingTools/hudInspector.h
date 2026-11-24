#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>

bool hudInspectorInspect(const GTImage *frame, GTRect hudRegion,
                         GTImageRegionStats *stats);
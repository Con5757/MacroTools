#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>

bool gameMenuDetectorIsVisible(const GTImage *frame,
                               const GTImage *menuTemplate,
                               double maximumMeanDifference,
                               GTImageMatch *match);
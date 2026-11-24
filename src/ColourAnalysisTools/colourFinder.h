#pragma once

#include "pixelToolsCommon.h"

BOOL colourFinderSearch(int x, int y, int width, int height, PixelColor target,
                        double tolerance, PixelPointArray *matches);
BOOL colourFinderSearchVirtualDesktop(PixelColor target, double tolerance,
                                      PixelPointArray *matches);
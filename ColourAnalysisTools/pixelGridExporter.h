#pragma once

#include "pixelToolsCommon.h"

BOOL pixelGridExporterWriteCSV(FILE *output, int x, int y, int width,
                               int height);
BOOL pixelGridExporterWriteHexGrid(FILE *output, int x, int y, int width,
                                   int height);
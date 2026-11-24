#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  int x;
  int y;
  int width;
  int height;
  double centroidX;
  double centroidY;
  size_t pixelCount;
  PixelColor averageColor;
} PixelColourCluster;

typedef struct {
  PixelColourCluster *items;
  size_t count;
} PixelColourClusterArray;
BOOL colourClusterDetectorFind(int x, int y, int width, int height,
                               PixelColor target, double tolerance,
                               size_t minimumClusterSize,
                               PixelColourClusterArray *clusters);
void colourClusterDetectorFree(PixelColourClusterArray *clusters);
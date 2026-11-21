#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  PixelColor rgb;
  char hex[8];
  double hsvHue;
  double hsvSaturation;
  double hsvValue;
  double hslHue;
  double hslSaturation;
  double hslLightness;
  double cmykCyan;
  double cmykMagenta;
  double cmykYellow;
  double cmykBlack;
} PixelInspectorData;

BOOL pixelInspectorGet(int x, int y, PixelInspectorData *data);
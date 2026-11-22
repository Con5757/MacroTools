#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  VT_SCAN_RGB_TOLERANCE,
  VT_SCAN_BRIGHTNESS_RANGE
} VTRegionScanMode;
typedef struct {
  VTRegionScanMode mode;
  VTRgb target;
  uint8_t channelTolerance;
  uint8_t minimumBrightness;
  uint8_t maximumBrightness;
} VTRegionScanConfig;
typedef struct {
  size_t matchedPixels;
  VTRect bounds;
  double coverage;
} VTRegionScanResult;

VTStatus vtRegionScan(const VTImage *image, VTRect region,
                      const VTRegionScanConfig *config,
                      VTRegionScanResult *result);

#ifdef __cplusplus
}
#endif
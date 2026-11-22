#include "regionScanner.h"

VTStatus vtRegionScan(const VTImage *image, VTRect region,
                      const VTRegionScanConfig *config,
                      VTRegionScanResult *result) {
  if (!vtImageIsValid(image) || config == NULL || result == NULL ||
      !vtRectIsValid(region) || region.x < 0 || region.y < 0 ||
      region.x > image->width - region.width ||
      region.y > image->height - region.height ||
      (config->mode != VT_SCAN_RGB_TOLERANCE &&
       config->mode != VT_SCAN_BRIGHTNESS_RANGE) ||
      config->minimumBrightness > config->maximumBrightness)
    return VT_STATUS_INVALID_ARGUMENT;
  size_t count = 0;
  int minX = region.x + region.width, minY = region.y + region.height;
  int maxX = -1, maxY = -1;
  for (int y = region.y; y < region.y + region.height; ++y) {
    for (int x = region.x; x < region.x + region.width; ++x) {
      VTRgb c;
      vtImageGetPixel(image, x, y, &c);
      BOOL match;
      if (config->mode == VT_SCAN_RGB_TOLERANCE) {
        match = c.r >= (config->target.r > config->channelTolerance
                            ? config->target.r - config->channelTolerance
                            : 0) &&
                c.r <= (config->target.r + config->channelTolerance < 255
                            ? config->target.r + config->channelTolerance
                            : 255) &&
                c.g >= (config->target.g > config->channelTolerance
                            ? config->target.g - config->channelTolerance
                            : 0) &&
                c.g <= (config->target.g + config->channelTolerance < 255
                            ? config->target.g + config->channelTolerance
                            : 255) &&
                c.b >= (config->target.b > config->channelTolerance
                            ? config->target.b - config->channelTolerance
                            : 0) &&
                c.b <= (config->target.b + config->channelTolerance < 255
                            ? config->target.b + config->channelTolerance
                            : 255);
      } else {
        unsigned int brightness = vtRgbBrightness(c);
        match = brightness >= config->minimumBrightness &&
                brightness <= config->maximumBrightness;
      }
      if (match) {
        ++count;
        if (x < minX)
          minX = x;
        if (y < minY)
          minY = y;
        if (x > maxX)
          maxX = x;
        if (y > maxY)
          maxY = y;
      }
    }
  }
  result->matchedPixels = count;
  result->coverage = (double)count / ((double)region.width * region.height);
  result->bounds = count
                       ? (VTRect){minX, minY, maxX - minX + 1, maxY - minY + 1}
                       : (VTRect){0, 0, 0, 0};
  return VT_STATUS_OK;
}
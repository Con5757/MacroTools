#include "pixelInspector.h"

BOOL pixelInspectorGet(int x, int y, PixelInspectorData *data) {
  if (data == NULL || !pixelReadColorAt(x, y, &data->rgb)) {
    return FALSE;
  }

  pixelColorToHex(data->rgb, data->hex);
  pixelColorToHSV(data->rgb, &data->hsvHue, &data->hsvSaturation,
                  &data->hsvValue);
  pixelColorToHSL(data->rgb, &data->hslHue, &data->hslSaturation,
                  &data->hslLightness);
  pixelColorToCMYK(data->rgb, &data->cmykCyan, &data->cmykMagenta,
                   &data->cmykYellow, &data->cmykBlack);
  return TRUE;
}
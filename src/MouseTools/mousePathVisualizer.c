#define WIN32_LEAN_AND_MEAN
#include "mousePathVisualizer.h"

BOOL mousePathVisualizerGetBounds(const MouseRecorderSample *samples,
                                  size_t sampleCount, MousePathBounds *bounds) {
  if (samples == NULL || sampleCount == 0 || bounds == NULL) {
    return FALSE;
  }

  bounds->left = samples[0].position.x;
  bounds->right = samples[0].position.x;
  bounds->top = samples[0].position.y;
  bounds->bottom = samples[0].position.y;
  bounds->valid = TRUE;

  for (size_t i = 1; i < sampleCount; ++i) {
    POINT p = samples[i].position;
    if (p.x < bounds->left)
      bounds->left = p.x;
    if (p.x > bounds->right)
      bounds->right = p.x;
    if (p.y < bounds->top)
      bounds->top = p.y;
    if (p.y > bounds->bottom)
      bounds->bottom = p.y;
  }

  return TRUE;
}

BOOL mousePathVisualizerMapPoint(POINT desktopPoint,
                                 const MousePathBounds *pathBounds,
                                 RECT targetBounds, POINT *targetPoint) {
  if (pathBounds == NULL || !pathBounds->valid || targetPoint == NULL) {
    return FALSE;
  }

  LONG targetWidth = targetBounds.right - targetBounds.left;
  LONG targetHeight = targetBounds.bottom - targetBounds.top;
  LONG pathWidth = pathBounds->right - pathBounds->left;
  LONG pathHeight = pathBounds->bottom - pathBounds->top;

  if (targetWidth <= 0 || targetHeight <= 0 || pathWidth < 0 ||
      pathHeight < 0) {
    return FALSE;
  }

  double scaleX = pathWidth > 0 ? (double)(targetWidth - 1) / pathWidth : 1.0;
  double scaleY =
      pathHeight > 0 ? (double)(targetHeight - 1) / pathHeight : 1.0;
  double scale = scaleX < scaleY ? scaleX : scaleY;

  double renderedWidth = pathWidth * scale;
  double renderedHeight = pathHeight * scale;
  double offsetX =
      targetBounds.left + ((targetWidth - 1) - renderedWidth) / 2.0;
  double offsetY =
      targetBounds.top + ((targetHeight - 1) - renderedHeight) / 2.0;

  double mappedX = offsetX + (desktopPoint.x - pathBounds->left) * scale;
  double mappedY = offsetY + (desktopPoint.y - pathBounds->top) * scale;

  targetPoint->x = (LONG)(mappedX + 0.5);
  targetPoint->y = (LONG)(mappedY + 0.5);
  return TRUE;
}
#include "shapeDetector.h"
#include <stdlib.h>
#include <string.h>

static BOOL shapeForeground(const VTImage *im, int x, int y,
                            uint8_t threshold) {
  VTRgb c;
  vtImageGetPixel(im, x, y, &c);
  return vtRgbBrightness(c) >= threshold;
}
VTStatus vtDetectShapes(const VTImage *im, uint8_t threshold,
                        size_t minimumPixels, VTDetectedShape *shapes,
                        size_t capacity, size_t *shapeCount) {
  if (!vtImageIsValid(im) || shapes == NULL || capacity == 0 ||
      shapeCount == NULL || minimumPixels == 0)
    return VT_STATUS_INVALID_ARGUMENT;
  if ((size_t)im->width > SIZE_MAX / (size_t)im->height)
    return VT_STATUS_INVALID_ARGUMENT;
  size_t total = (size_t)im->width * (size_t)im->height;
  if (total > SIZE_MAX / sizeof(size_t))
    return VT_STATUS_INVALID_ARGUMENT;
  uint8_t *visited = (uint8_t *)calloc(total, 1u);
  size_t *queue = (size_t *)malloc(total * sizeof(size_t));
  if (!visited || !queue) {
    free(visited);
    free(queue);
    return VT_STATUS_OUT_OF_MEMORY;
  }
  size_t found = 0;
  for (int sy = 0; sy < im->height; ++sy)
    for (int sx = 0; sx < im->width; ++sx) {
      size_t start = (size_t)sy * (size_t)im->width + (size_t)sx;
      if (visited[start] || !shapeForeground(im, sx, sy, threshold))
        continue;
      size_t head = 0, tail = 0;
      queue[tail++] = start;
      visited[start] = 1;
      int minX = sx, maxX = sx, minY = sy, maxY = sy;
      size_t pixels = 0;
      while (head < tail) {
        size_t idx = queue[head++];
        int x = (int)(idx % (size_t)im->width),
            y = (int)(idx / (size_t)im->width);
        ++pixels;
        if (x < minX)
          minX = x;
        if (x > maxX)
          maxX = x;
        if (y < minY)
          minY = y;
        if (y > maxY)
          maxY = y;
        for (int dy = -1; dy <= 1; ++dy)
          for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0)
              continue;
            int nx = x + dx, ny = y + dy;
            if (nx < 0 || ny < 0 || nx >= im->width || ny >= im->height)
              continue;
            size_t ni = (size_t)ny * (size_t)im->width + (size_t)nx;
            if (!visited[ni] && shapeForeground(im, nx, ny, threshold)) {
              visited[ni] = 1;
              queue[tail++] = ni;
            }
          }
      }
      if (pixels < minimumPixels || found >= capacity)
        continue;
      int w = maxX - minX + 1, h = maxY - minY + 1;
      double fill = (double)pixels / ((double)w * h);
      double aspect = (double)w / (double)h;
      VTShapeType type = VT_SHAPE_BLOB;
      if (aspect > 5.0 || aspect < 0.2)
        type = VT_SHAPE_LINE;
      else if (fill > 0.88)
        type = VT_SHAPE_RECTANGLE;
      else if (aspect > 0.65 && aspect < 1.5 && fill > 0.55 && fill < 0.88)
        type = VT_SHAPE_ELLIPSE;
      shapes[found++] =
          (VTDetectedShape){{minX, minY, w, h}, type, pixels, fill};
    }
  free(visited);
  free(queue);
  *shapeCount = found;
  return VT_STATUS_OK;
}
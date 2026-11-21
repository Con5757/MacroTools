#include "colourClusterDetector.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

void colourClusterDetectorFree(PixelColourClusterArray *clusters) {
  if (clusters == NULL)
    return;
  free(clusters->items);
  clusters->items = NULL;
  clusters->count = 0;
}

static BOOL matchesTarget(PixelColor color, PixelColor target,
                          double toleranceSquared) {
  double dr = (double)color.r - target.r;
  double dg = (double)color.g - target.g;
  double db = (double)color.b - target.b;
  return dr * dr + dg * dg + db * db <= toleranceSquared;
}

static BOOL appendCluster(PixelColourClusterArray *clusters,
                          PixelColourCluster cluster, size_t *capacity) {
  if (clusters->count == *capacity) {
    size_t newCapacity = *capacity == 0 ? 16u : *capacity * 2u;
    if (newCapacity < *capacity ||
        newCapacity > SIZE_MAX / sizeof(PixelColourCluster))
      return FALSE;
    PixelColourCluster *items = (PixelColourCluster *)realloc(
        clusters->items, newCapacity * sizeof(PixelColourCluster));
    if (items == NULL)
      return FALSE;
    clusters->items = items;
    *capacity = newCapacity;
  }
  clusters->items[clusters->count++] = cluster;
  return TRUE;
}

BOOL colourClusterDetectorFind(int x, int y, int width, int height,
                               PixelColor target, double tolerance,
                               size_t minimumClusterSize,
                               PixelColourClusterArray *clusters) {
  if (clusters == NULL || !isfinite(tolerance) || tolerance < 0.0 ||
      minimumClusterSize == 0)
    return FALSE;
  colourClusterDetectorFree(clusters);

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  size_t total = (size_t)image.width * (size_t)image.height;
  if (total > SIZE_MAX / sizeof(size_t)) {
    pixelImageDestroy(&image);
    return FALSE;
  }

  BYTE *visited = (BYTE *)calloc(total, sizeof(BYTE));
  size_t *queue = (size_t *)malloc(total * sizeof(size_t));
  if (visited == NULL || queue == NULL) {
    free(visited);
    free(queue);
    pixelImageDestroy(&image);
    return FALSE;
  }

  double toleranceSquared = tolerance * tolerance;
  size_t clusterCapacity = 0;
  size_t seed;
  BOOL success = TRUE;

  for (seed = 0; seed < total && success; ++seed) {
    if (visited[seed] ||
        !matchesTarget(image.pixels[seed], target, toleranceSquared)) {
      continue;
    }

    size_t head = 0, tail = 0;
    queue[tail++] = seed;
    visited[seed] = 1;

    size_t count = 0;
    int minX = image.width, minY = image.height, maxX = -1, maxY = -1;
    uint64_t sumR = 0, sumG = 0, sumB = 0;
    uint64_t sumX = 0, sumY = 0;

    while (head < tail) {
      size_t index = queue[head++];
      int px = (int)(index % (size_t)image.width);
      int py = (int)(index / (size_t)image.width);
      PixelColor c = image.pixels[index];

      ++count;
      if (px < minX)
        minX = px;
      if (py < minY)
        minY = py;
      if (px > maxX)
        maxX = px;
      if (py > maxY)
        maxY = py;
      sumR += c.r;
      sumG += c.g;
      sumB += c.b;
      sumX += (uint64_t)px;
      sumY += (uint64_t)py;

      size_t neighbors[4];
      size_t neighborCount = 0;
      if (px > 0)
        neighbors[neighborCount++] = index - 1;
      if (px + 1 < image.width)
        neighbors[neighborCount++] = index + 1;
      if (py > 0)
        neighbors[neighborCount++] = index - (size_t)image.width;
      if (py + 1 < image.height)
        neighbors[neighborCount++] = index + (size_t)image.width;

      size_t n;
      for (n = 0; n < neighborCount; ++n) {
        size_t next = neighbors[n];
        if (!visited[next] &&
            matchesTarget(image.pixels[next], target, toleranceSquared)) {
          visited[next] = 1;
          queue[tail++] = next;
        }
      }
    }

    if (count >= minimumClusterSize) {
      PixelColourCluster cluster;
      cluster.x = image.x + minX;
      cluster.y = image.y + minY;
      cluster.width = maxX - minX + 1;
      cluster.height = maxY - minY + 1;
      cluster.centroidX = (double)image.x + (double)sumX / (double)count;
      cluster.centroidY = (double)image.y + (double)sumY / (double)count;
      cluster.pixelCount = count;
      cluster.averageColor.r = (BYTE)(sumR / count);
      cluster.averageColor.g = (BYTE)(sumG / count);
      cluster.averageColor.b = (BYTE)(sumB / count);
      if (!appendCluster(clusters, cluster, &clusterCapacity))
        success = FALSE;
    }
  }

  free(visited);
  free(queue);
  pixelImageDestroy(&image);
  if (!success)
    colourClusterDetectorFree(clusters);
  return success;
}

#include "colourFrequencyAnalyzer.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t key;
  size_t count;
  uint64_t sumR;
  uint64_t sumG;
  uint64_t sumB;
  BOOL occupied;
} FrequencySlot;

static uint32_t hashColour(uint32_t value) {
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  value *= 0x846ca68bU;
  value ^= value >> 16;
  return value;
}

static int compareFrequency(const void *left, const void *right) {
  const PixelColourFrequency *a = (const PixelColourFrequency *)left;
  const PixelColourFrequency *b = (const PixelColourFrequency *)right;
  if (a->count < b->count)
    return 1;
  if (a->count > b->count)
    return -1;
  if (a->color.r != b->color.r)
    return (int)a->color.r - (int)b->color.r;
  if (a->color.g != b->color.g)
    return (int)a->color.g - (int)b->color.g;
  return (int)a->color.b - (int)b->color.b;
}

BOOL colourFrequencyAnalyzerAnalyze(int x, int y, int width, int height,
                                    unsigned int bitsToDiscardPerChannel,
                                    size_t maxResults,
                                    PixelColourFrequency **frequencies,
                                    size_t *frequencyCount) {
  if (frequencies == NULL || frequencyCount == NULL ||
      bitsToDiscardPerChannel > 7u)
    return FALSE;
  *frequencies = NULL;
  *frequencyCount = 0;

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  size_t total = (size_t)image.width * (size_t)image.height;
  if (total > SIZE_MAX / 2u) {
    pixelImageDestroy(&image);
    return FALSE;
  }

  size_t tableCapacity = 16u;
  while (tableCapacity < total * 2u) {
    if (tableCapacity > SIZE_MAX / 2u) {
      pixelImageDestroy(&image);
      return FALSE;
    }
    tableCapacity *= 2u;
  }
  if (tableCapacity > SIZE_MAX / sizeof(FrequencySlot)) {
    pixelImageDestroy(&image);
    return FALSE;
  }

  FrequencySlot *table =
      (FrequencySlot *)calloc(tableCapacity, sizeof(FrequencySlot));
  if (table == NULL) {
    pixelImageDestroy(&image);
    return FALSE;
  }

  unsigned int shift = bitsToDiscardPerChannel;
  size_t i, uniqueCount = 0;
  for (i = 0; i < total; ++i) {
    PixelColor c = image.pixels[i];
    uint32_t qr = (uint32_t)(c.r >> shift);
    uint32_t qg = (uint32_t)(c.g >> shift);
    uint32_t qb = (uint32_t)(c.b >> shift);
    uint32_t key = (qr << 16) | (qg << 8) | qb;
    size_t slot = (size_t)hashColour(key) & (tableCapacity - 1u);

    while (table[slot].occupied && table[slot].key != key) {
      slot = (slot + 1u) & (tableCapacity - 1u);
    }
    if (!table[slot].occupied) {
      table[slot].occupied = TRUE;
      table[slot].key = key;
      ++uniqueCount;
    }
    ++table[slot].count;
    table[slot].sumR += c.r;
    table[slot].sumG += c.g;
    table[slot].sumB += c.b;
  }

  PixelColourFrequency *items = (PixelColourFrequency *)malloc(
      uniqueCount * sizeof(PixelColourFrequency));
  if (items == NULL) {
    free(table);
    pixelImageDestroy(&image);
    return FALSE;
  }

  size_t out = 0;
  for (i = 0; i < tableCapacity; ++i) {
    if (!table[i].occupied)
      continue;
    items[out].count = table[i].count;
    items[out].percentage = (double)table[i].count / (double)total * 100.0;
    items[out].color.r = (BYTE)(table[i].sumR / table[i].count);
    items[out].color.g = (BYTE)(table[i].sumG / table[i].count);
    items[out].color.b = (BYTE)(table[i].sumB / table[i].count);
    ++out;
  }

  qsort(items, uniqueCount, sizeof(PixelColourFrequency), compareFrequency);
  size_t resultCount =
      maxResults == 0 || maxResults > uniqueCount ? uniqueCount : maxResults;
  if (resultCount < uniqueCount) {
    PixelColourFrequency *trimmed = (PixelColourFrequency *)realloc(
        items, resultCount * sizeof(PixelColourFrequency));
    if (trimmed != NULL)
      items = trimmed;
  }

  free(table);
  pixelImageDestroy(&image);
  *frequencies = items;
  *frequencyCount = resultCount;
  return TRUE;
}
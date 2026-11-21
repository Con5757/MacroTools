#include "pixelGridExporter.h"

BOOL pixelGridExporterWriteCSV(FILE *output, int x, int y, int width,
                               int height) {
  if (output == NULL)
    return FALSE;

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  BOOL success = fprintf(output, "desktop_x,desktop_y,r,g,b,hex\n") >= 0;
  int row, column;
  for (row = 0; row < image.height && success; ++row) {
    for (column = 0; column < image.width; ++column) {
      PixelColor c =
          image.pixels[(size_t)row * (size_t)image.width + (size_t)column];
      char hex[8];
      pixelColorToHex(c, hex);
      if (fprintf(output, "%d,%d,%u,%u,%u,%s\n", image.x + column,
                  image.y + row, (unsigned)c.r, (unsigned)c.g, (unsigned)c.b,
                  hex) < 0) {
        success = FALSE;
        break;
      }
    }
  }

  pixelImageDestroy(&image);
  return success && !ferror(output);
}

BOOL pixelGridExporterWriteHexGrid(FILE *output, int x, int y, int width,
                                   int height) {
  if (output == NULL)
    return FALSE;

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  BOOL success = TRUE;
  int row, column;
  for (row = 0; row < image.height && success; ++row) {
    for (column = 0; column < image.width; ++column) {
      PixelColor c =
          image.pixels[(size_t)row * (size_t)image.width + (size_t)column];
      char hex[8];
      pixelColorToHex(c, hex);
      if (fprintf(output, "%s%s", column == 0 ? "" : " ", hex) < 0) {
        success = FALSE;
        break;
      }
    }
    if (success && fputc('\n', output) == EOF)
      success = FALSE;
  }

  pixelImageDestroy(&image);
  return success && !ferror(output);
}
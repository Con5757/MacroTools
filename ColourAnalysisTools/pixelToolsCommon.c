#define WIN32_LEAN_AND_MEAN
#include "pixelToolsCommon.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

BOOL pixelRegionIsValid(int x, int y, int width, int height) {
  if (width <= 0 || height <= 0) {
    return FALSE;
  }

  int64_t right = (int64_t)x + (int64_t)width;
  int64_t bottom = (int64_t)y + (int64_t)height;

  if (right > INT_MAX || right < INT_MIN || bottom > INT_MAX ||
      bottom < INT_MIN) {
    return FALSE;
  }

  return TRUE;
}

BOOL pixelImageCapture(int x, int y, int width, int height, PixelImage *image) {
  if (image == NULL) {
    return FALSE;
  }

  ZeroMemory(image, sizeof(*image));
  if (!pixelRegionIsValid(x, y, width, height)) {
    return FALSE;
  }

  image->x = 0;
  image->y = 0;
  image->width = 0;
  image->height = 0;
  image->pixels = NULL;

  if ((size_t)width > SIZE_MAX / (size_t)height ||
      (size_t)width > SIZE_MAX / 4u / (size_t)height) {
    return FALSE;
  }
  size_t pixelCount = (size_t)width * (size_t)height;
  if (pixelCount > SIZE_MAX / sizeof(PixelColor)) {
    return FALSE;
  }

  HDC screenDC = GetDC(NULL);
  if (screenDC == NULL) {
    return FALSE;
  }

  HDC memoryDC = CreateCompatibleDC(screenDC);
  if (memoryDC == NULL) {
    ReleaseDC(NULL, screenDC);
    return FALSE;
  }

  BITMAPINFO bitmapInfo;
  ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
  bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmapInfo.bmiHeader.biWidth = width;
  bitmapInfo.bmiHeader.biHeight = -height; /* top-down DIB */
  bitmapInfo.bmiHeader.biPlanes = 1;
  bitmapInfo.bmiHeader.biBitCount = 32;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;

  void *dibBits = NULL;
  HBITMAP bitmap = CreateDIBSection(screenDC, &bitmapInfo, DIB_RGB_COLORS,
                                    &dibBits, NULL, 0);
  if (bitmap == NULL || dibBits == NULL) {
    if (bitmap != NULL) {
      DeleteObject(bitmap);
    }
    DeleteDC(memoryDC);
    ReleaseDC(NULL, screenDC);
    return FALSE;
  }

  HGDIOBJ previousObject = SelectObject(memoryDC, bitmap);
  BOOL copied = FALSE;
  if (previousObject != NULL && previousObject != HGDI_ERROR) {
    copied = BitBlt(memoryDC, 0, 0, width, height, screenDC, x, y,
                    SRCCOPY | CAPTUREBLT);
  }

  PixelColor *pixels = NULL;
  if (copied) {
    pixels = (PixelColor *)malloc(pixelCount * sizeof(PixelColor));
    if (pixels != NULL) {
      const BYTE *source = (const BYTE *)dibBits;
      size_t i;
      for (i = 0; i < pixelCount; ++i) {
        pixels[i].b = source[i * 4u + 0u];
        pixels[i].g = source[i * 4u + 1u];
        pixels[i].r = source[i * 4u + 2u];
      }
    }
  }

  if (previousObject != NULL && previousObject != HGDI_ERROR) {
    SelectObject(memoryDC, previousObject);
  }
  DeleteObject(bitmap);
  DeleteDC(memoryDC);
  ReleaseDC(NULL, screenDC);

  if (!copied || pixels == NULL) {
    free(pixels);
    return FALSE;
  }

  image->x = x;
  image->y = y;
  image->width = width;
  image->height = height;
  image->pixels = pixels;
  return TRUE;
}

BOOL pixelImageCaptureVirtualDesktop(PixelImage *image) {
  int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

  return pixelImageCapture(x, y, width, height, image);
}

void pixelImageDestroy(PixelImage *image) {
  if (image == NULL) {
    return;
  }

  free(image->pixels);
  ZeroMemory(image, sizeof(*image));
}

BOOL pixelReadColorAt(int x, int y, PixelColor *color) {
  if (color == NULL) {
    return FALSE;
  }

  PixelImage image;
  if (!pixelImageCapture(x, y, 1, 1, &image)) {
    return FALSE;
  }

  *color = image.pixels[0];
  pixelImageDestroy(&image);
  return TRUE;
}

void pixelColorToHex(PixelColor color, char hex[8]) {
  if (hex != NULL) {
    (void)snprintf(hex, 8, "#%02X%02X%02X", color.r, color.g, color.b);
  }
}

void pixelColorToHSV(PixelColor color, double *hue, double *saturation,
                     double *value) {
  double r = color.r / 255.0;
  double g = color.g / 255.0;
  double b = color.b / 255.0;
  double maximum = fmax(r, fmax(g, b));
  double minimum = fmin(r, fmin(g, b));
  double delta = maximum - minimum;
  double h = 0.0;

  if (delta > 0.0) {
    if (maximum == r) {
      h = 60.0 * fmod((g - b) / delta, 6.0);
    } else if (maximum == g) {
      h = 60.0 * (((b - r) / delta) + 2.0);
    } else {
      h = 60.0 * (((r - g) / delta) + 4.0);
    }
    if (h < 0.0)
      h += 360.0;
  }

  if (hue != NULL)
    *hue = h;
  if (saturation != NULL)
    *saturation = maximum <= 0.0 ? 0.0 : delta / maximum;
  if (value != NULL)
    *value = maximum;
}

void pixelColorToHSL(PixelColor color, double *hue, double *saturation,
                     double *lightness) {
  double r = color.r / 255.0;
  double g = color.g / 255.0;
  double b = color.b / 255.0;
  double maximum = fmax(r, fmax(g, b));
  double minimum = fmin(r, fmin(g, b));
  double delta = maximum - minimum;
  double h = 0.0;
  double l = (maximum + minimum) / 2.0;
  double s = 0.0;

  if (delta > 0.0) {
    s = delta / (1.0 - fabs(2.0 * l - 1.0));
    if (maximum == r) {
      h = 60.0 * fmod((g - b) / delta, 6.0);
    } else if (maximum == g) {
      h = 60.0 * (((b - r) / delta) + 2.0);
    } else {
      h = 60.0 * (((r - g) / delta) + 4.0);
    }
    if (h < 0.0)
      h += 360.0;
  }

  if (hue != NULL)
    *hue = h;
  if (saturation != NULL)
    *saturation = s;
  if (lightness != NULL)
    *lightness = l;
}

void pixelColorToCMYK(PixelColor color, double *cyan, double *magenta,
                      double *yellow, double *black) {
  double r = color.r / 255.0;
  double g = color.g / 255.0;
  double b = color.b / 255.0;
  double k = 1.0 - fmax(r, fmax(g, b));
  double c = 0.0, m = 0.0, y = 0.0;

  if (k < 1.0) {
    c = (1.0 - r - k) / (1.0 - k);
    m = (1.0 - g - k) / (1.0 - k);
    y = (1.0 - b - k) / (1.0 - k);
  }

  if (cyan != NULL)
    *cyan = c;
  if (magenta != NULL)
    *magenta = m;
  if (yellow != NULL)
    *yellow = y;
  if (black != NULL)
    *black = k;
}

double pixelColorDistance(PixelColor a, PixelColor b) {
  double dr = (double)a.r - b.r;
  double dg = (double)a.g - b.g;
  double db = (double)a.b - b.b;
  return sqrt(dr * dr + dg * dg + db * db);
}

BOOL pixelPointArrayAppend(PixelPointArray *array, PixelPoint point) {
  if (array == NULL) {
    return FALSE;
  }

  if (array->count == array->capacity) {
    size_t newCapacity = array->capacity == 0 ? 64u : array->capacity * 2u;
    if (newCapacity < array->capacity ||
        newCapacity > SIZE_MAX / sizeof(PixelPoint)) {
      return FALSE;
    }

    PixelPoint *newItems =
        (PixelPoint *)realloc(array->items, newCapacity * sizeof(PixelPoint));
    if (newItems == NULL) {
      return FALSE;
    }

    array->items = newItems;
    array->capacity = newCapacity;
  }

  array->items[array->count++] = point;
  return TRUE;
}

void pixelPointArrayDestroy(PixelPointArray *array) {
  if (array == NULL) {
    return;
  }

  free(array->items);
  ZeroMemory(array, sizeof(*array));
}
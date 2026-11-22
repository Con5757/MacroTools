#include "visualToolsCommon.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

BOOL vtImageIsValid(const VTImage *image) {
  return image != NULL && image->pixels != NULL && image->width > 0 &&
         image->height > 0 && image->width <= INT_MAX / 3 &&
         image->stride >= image->width * 3;
}
VTStatus vtImageCreate(VTImage *image, int width, int height) {
  if (image == NULL || width <= 0 || height <= 0 || width > INT_MAX / 3 ||
      (size_t)width * 3u > SIZE_MAX / (size_t)height) {
    return VT_STATUS_INVALID_ARGUMENT;
  }
  size_t bytes = (size_t)width * (size_t)height * 3u;
  uint8_t *pixels = (uint8_t *)calloc(bytes, 1u);
  if (pixels == NULL)
    return VT_STATUS_OUT_OF_MEMORY;
  vtImageDestroy(image);
  image->width = width;
  image->height = height;
  image->stride = width * 3;
  image->pixels = pixels;
  return VT_STATUS_OK;
}
VTStatus vtImageClone(const VTImage *source, VTImage *destination) {
  if (!vtImageIsValid(source) || destination == NULL)
    return VT_STATUS_INVALID_ARGUMENT;
  if (source == destination)
    return VT_STATUS_OK;
  VTStatus status = vtImageCreate(destination, source->width, source->height);
  if (status != VT_STATUS_OK)
    return status;
  for (int y = 0; y < source->height; ++y) {
    memcpy(destination->pixels + (size_t)y * (size_t)destination->stride,
           source->pixels + (size_t)y * (size_t)source->stride,
           (size_t)source->width * 3u);
  }
  return VT_STATUS_OK;
}
BOOL vtRectIsValid(VTRect rect) {
  return rect.width > 0 && rect.height > 0 && rect.x <= INT_MAX - rect.width &&
         rect.y <= INT_MAX - rect.height;
}
VTStatus vtImageCrop(const VTImage *source, VTRect region,
                     VTImage *destination) {
  if (!vtImageIsValid(source) || destination == NULL || source == destination ||
      !vtRectIsValid(region) || region.x < 0 || region.y < 0 ||
      region.x > source->width - region.width ||
      region.y > source->height - region.height) {
    return VT_STATUS_INVALID_ARGUMENT;
  }
  VTStatus status = vtImageCreate(destination, region.width, region.height);
  if (status != VT_STATUS_OK)
    return status;
  for (int y = 0; y < region.height; ++y) {
    const uint8_t *src = source->pixels +
                         (size_t)(region.y + y) * (size_t)source->stride +
                         (size_t)region.x * 3u;
    uint8_t *dst =
        destination->pixels + (size_t)y * (size_t)destination->stride;
    memcpy(dst, src, (size_t)region.width * 3u);
  }
  return VT_STATUS_OK;
}
void vtImageDestroy(VTImage *image) {
  if (image == NULL)
    return;
  free(image->pixels);
  image->pixels = NULL;
  image->width = image->height = image->stride = 0;
}
BOOL vtImageGetPixel(const VTImage *image, int x, int y, VTRgb *color) {
  if (!vtImageIsValid(image) || color == NULL || x < 0 || y < 0 ||
      x >= image->width || y >= image->height)
    return FALSE;
  const uint8_t *p =
      image->pixels + (size_t)y * (size_t)image->stride + (size_t)x * 3u;
  color->r = p[0];
  color->g = p[1];
  color->b = p[2];
  return TRUE;
}
BOOL vtImageSetPixel(VTImage *image, int x, int y, VTRgb color) {
  if (!vtImageIsValid(image) || x < 0 || y < 0 || x >= image->width ||
      y >= image->height)
    return FALSE;
  uint8_t *p =
      image->pixels + (size_t)y * (size_t)image->stride + (size_t)x * 3u;
  p[0] = color.r;
  p[1] = color.g;
  p[2] = color.b;
  return TRUE;
}
VTStatus vtImageCaptureScreen(VTRect screenRegion, VTImage *destination) {
  if (destination == NULL || !vtRectIsValid(screenRegion) ||
      screenRegion.width > (INT_MAX - 3) / 3)
    return VT_STATUS_INVALID_ARGUMENT;

  HDC screenDC = GetDC(NULL);
  if (screenDC == NULL)
    return VT_STATUS_CAPTURE_FAILED;
  HDC memoryDC = CreateCompatibleDC(screenDC);
  if (memoryDC == NULL) {
    ReleaseDC(NULL, screenDC);
    return VT_STATUS_CAPTURE_FAILED;
  }
  HBITMAP bitmap =
      CreateCompatibleBitmap(screenDC, screenRegion.width, screenRegion.height);
  if (bitmap == NULL) {
    DeleteDC(memoryDC);
    ReleaseDC(NULL, screenDC);
    return VT_STATUS_CAPTURE_FAILED;
  }
  HGDIOBJ oldObject = SelectObject(memoryDC, bitmap);
  BOOL copied =
      oldObject != NULL && oldObject != HGDI_ERROR &&
      BitBlt(memoryDC, 0, 0, screenRegion.width, screenRegion.height, screenDC,
             screenRegion.x, screenRegion.y, SRCCOPY | CAPTUREBLT);

  VTStatus status = VT_STATUS_CAPTURE_FAILED;
  if (copied) {
    // bitmap needs to be deselected from the memory DC for GetDIBits
    HGDIOBJ restoredObject = SelectObject(memoryDC, oldObject);
    if (restoredObject == NULL || restoredObject == HGDI_ERROR) {
      copied = FALSE;
    } else {
      oldObject = NULL;
    }
  }

  if (copied) {
    BITMAPINFO info;
    ZeroMemory(&info, sizeof(info));
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = screenRegion.width;
    info.bmiHeader.biHeight = -screenRegion.height; /* top-down */
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biCompression = BI_RGB;
    int rawStride = ((screenRegion.width * 3 + 3) / 4) * 4;
    if (screenRegion.height > 0 &&
        (size_t)rawStride <= SIZE_MAX / (size_t)screenRegion.height) {
      size_t rawBytes = (size_t)rawStride * (size_t)screenRegion.height;
      uint8_t *raw = (uint8_t *)malloc(rawBytes);
      if (raw != NULL) {
        if (GetDIBits(memoryDC, bitmap, 0, (UINT)screenRegion.height, raw,
                      &info, DIB_RGB_COLORS) == (int)screenRegion.height) {
          VTImage temp = {0};
          status =
              vtImageCreate(&temp, screenRegion.width, screenRegion.height);
          if (status == VT_STATUS_OK) {
            for (int y = 0; y < screenRegion.height; ++y) {
              const uint8_t *src = raw + (size_t)y * (size_t)rawStride;
              uint8_t *dst = temp.pixels + (size_t)y * (size_t)temp.stride;
              for (int x = 0; x < screenRegion.width; ++x) {
                dst[x * 3 + 0] = src[x * 3 + 2];
                dst[x * 3 + 1] = src[x * 3 + 1];
                dst[x * 3 + 2] = src[x * 3 + 0];
              }
            }
            vtImageDestroy(destination);
            *destination = temp;
          }
        }
        free(raw);
      } else
        status = VT_STATUS_OUT_OF_MEMORY;
    }
  }

  if (oldObject != NULL && oldObject != HGDI_ERROR)
    SelectObject(memoryDC, oldObject);
  DeleteObject(bitmap);
  DeleteDC(memoryDC);
  ReleaseDC(NULL, screenDC);
  return status;
}
VTStatus vtImageCaptureDesktop(VTImage *destination) {
  VTRect bounds;
  bounds.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  bounds.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
  bounds.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  bounds.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  return vtImageCaptureScreen(bounds, destination);
}
unsigned int vtRgbManhattanDistance(VTRgb a, VTRgb b) {
  unsigned int dr = a.r > b.r ? a.r - b.r : b.r - a.r;
  unsigned int dg = a.g > b.g ? a.g - b.g : b.g - a.g;
  unsigned int db = a.b > b.b ? a.b - b.b : b.b - a.b;
  return dr + dg + db;
}
unsigned int vtRgbBrightness(VTRgb color) {
  return (299u * color.r + 587u * color.g + 114u * color.b) / 1000u;
}
double vtRectIntersectionOverUnion(VTRect a, VTRect b) {
  if (!vtRectIsValid(a) || !vtRectIsValid(b))
    return 0.0;
  int left = a.x > b.x ? a.x : b.x;
  int top = a.y > b.y ? a.y : b.y;
  int rightA = a.x + a.width, rightB = b.x + b.width;
  int bottomA = a.y + a.height, bottomB = b.y + b.height;
  int right = rightA < rightB ? rightA : rightB;
  int bottom = bottomA < bottomB ? bottomA : bottomB;
  int iw = right > left ? right - left : 0;
  int ih = bottom > top ? bottom - top : 0;
  double intersection = (double)iw * ih;
  double unionArea =
      (double)a.width * a.height + (double)b.width * b.height - intersection;
  return unionArea > 0.0 ? intersection / unionArea : 0.0;
}
VTStatus vtImageFindTemplate(const VTImage *haystack, const VTImage *needle,
                             VTRect searchRegion, int step, double *bestScore,
                             VTPoint *bestPosition) {
  if (!vtImageIsValid(haystack) || !vtImageIsValid(needle) ||
      bestScore == NULL || bestPosition == NULL ||
      !vtRectIsValid(searchRegion) || step < 1 || searchRegion.x < 0 ||
      searchRegion.y < 0 ||
      searchRegion.x > haystack->width - searchRegion.width ||
      searchRegion.y > haystack->height - searchRegion.height ||
      needle->width > searchRegion.width ||
      needle->height > searchRegion.height)
    return VT_STATUS_INVALID_ARGUMENT;

  double best = -1.0;
  VTPoint position = {0, 0};
  for (int y = searchRegion.y;
       y <= searchRegion.y + searchRegion.height - needle->height; y += step) {
    for (int x = searchRegion.x;
         x <= searchRegion.x + searchRegion.width - needle->width; x += step) {
      uint64_t difference = 0;
      uint64_t maximum =
          (uint64_t)needle->width * (uint64_t)needle->height * 765u;
      for (int ty = 0; ty < needle->height; ++ty) {
        const uint8_t *a = haystack->pixels +
                           (size_t)(y + ty) * (size_t)haystack->stride +
                           (size_t)x * 3u;
        const uint8_t *b = needle->pixels + (size_t)ty * (size_t)needle->stride;
        for (int tx = 0; tx < needle->width * 3; ++tx) {
          int delta = (int)a[tx] - (int)b[tx];
          difference += (uint64_t)(delta < 0 ? -delta : delta);
        }
      }
      double score = 1.0 - (double)difference / (double)maximum;
      if (score > best) {
        best = score;
        position.x = x;
        position.y = y;
      }
      if (best >= 1.0)
        break;
    }
    if (best >= 1.0)
      break;
  }
  if (best < 0.0)
    return VT_STATUS_NOT_FOUND;
  *bestScore = best;
  *bestPosition = position;
  return VT_STATUS_OK;
}
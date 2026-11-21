#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include "captureWic.h"

#include <Windows.h>
#include <limits.h>
#include <objbase.h>
#include <wincodec.h>

static BOOL utf8ToWidePath(const char *path, WCHAR output[MAX_PATH]) {
  if (path == NULL || path[0] == '\0')
    return FALSE;
  int count = MultiByteToWideChar(CP_UTF8, 0, path, -1, output, MAX_PATH);
  return count > 0 && count < MAX_PATH;
}

static HRESULT createWicFactory(IWICImagingFactory **factory) {
  return CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                          &IID_IWICImagingFactory, (void **)factory);
}

const char *captureImageFormatExtension(CaptureImageFormat format) {
  switch (format) {
  case CAPTURE_FORMAT_PNG:
    return "png";
  case CAPTURE_FORMAT_JPEG:
    return "jpg";
  case CAPTURE_FORMAT_BMP:
    return "bmp";
  default:
    return NULL;
  }
}

BOOL captureImageSave(const char *utf8Path, CaptureImageFormat format,
                      const CaptureImage *image) {
  if (image == NULL || image->pixels == NULL || image->width == 0 ||
      image->height == 0 || image->dataSize > UINT_MAX || image->stride == 0)
    return FALSE;

  WCHAR path[MAX_PATH];
  if (!utf8ToWidePath(utf8Path, path))
    return FALSE;

  HRESULT comResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  BOOL uninitializeCom = SUCCEEDED(comResult);
  if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE)
    return FALSE;

  BOOL success = FALSE;
  IWICImagingFactory *factory = NULL;
  IWICStream *stream = NULL;
  IWICBitmapEncoder *encoder = NULL;
  IWICBitmapFrameEncode *frame = NULL;
  IPropertyBag2 *propertyBag = NULL;
  IWICBitmap *bitmap = NULL;
  IWICFormatConverter *converter = NULL;

  const GUID *container = NULL;
  const GUID *pixelFormat = NULL;
  switch (format) {
  case CAPTURE_FORMAT_PNG:
    container = &GUID_ContainerFormatPng;
    pixelFormat = &GUID_WICPixelFormat32bppBGRA;
    break;
  case CAPTURE_FORMAT_JPEG:
    container = &GUID_ContainerFormatJpeg;
    pixelFormat = &GUID_WICPixelFormat24bppBGR;
    break;
  case CAPTURE_FORMAT_BMP:
    container = &GUID_ContainerFormatBmp;
    pixelFormat = &GUID_WICPixelFormat32bppBGRA;
    break;
  default:
    goto cleanup;
  }

  if (FAILED(createWicFactory(&factory)))
    goto cleanup;
  if (FAILED(IWICImagingFactory_CreateStream(factory, &stream)))
    goto cleanup;
  if (FAILED(IWICStream_InitializeFromFilename(stream, path, GENERIC_WRITE)))
    goto cleanup;
  if (FAILED(
          IWICImagingFactory_CreateEncoder(factory, container, NULL, &encoder)))
    goto cleanup;
  if (FAILED(IWICBitmapEncoder_Initialize(encoder, (IStream *)stream,
                                          WICBitmapEncoderNoCache)))
    goto cleanup;
  if (FAILED(IWICBitmapEncoder_CreateNewFrame(encoder, &frame, &propertyBag)))
    goto cleanup;
  if (FAILED(IWICBitmapFrameEncode_Initialize(frame, propertyBag)))
    goto cleanup;
  if (FAILED(IWICBitmapFrameEncode_SetSize(frame, image->width, image->height)))
    goto cleanup;

  GUID negotiatedFormat = *pixelFormat;
  if (FAILED(IWICBitmapFrameEncode_SetPixelFormat(frame, &negotiatedFormat)))
    goto cleanup;
  if (FAILED(IWICImagingFactory_CreateBitmapFromMemory(
          factory, image->width, image->height, &GUID_WICPixelFormat32bppBGRA,
          image->stride, (UINT)image->dataSize, image->pixels, &bitmap)))
    goto cleanup;
  if (FAILED(IWICImagingFactory_CreateFormatConverter(factory, &converter)))
    goto cleanup;
  if (FAILED(IWICFormatConverter_Initialize(
          converter, (IWICBitmapSource *)bitmap, &negotiatedFormat,
          WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom)))
    goto cleanup;
  if (FAILED(IWICBitmapFrameEncode_WriteSource(
          frame, (IWICBitmapSource *)converter, NULL)))
    goto cleanup;
  if (FAILED(IWICBitmapFrameEncode_Commit(frame)))
    goto cleanup;
  if (FAILED(IWICBitmapEncoder_Commit(encoder)))
    goto cleanup;
  success = TRUE;

cleanup:
  if (converter)
    IWICFormatConverter_Release(converter);
  if (bitmap)
    IWICBitmap_Release(bitmap);
  if (propertyBag)
    IPropertyBag2_Release(propertyBag);
  if (frame)
    IWICBitmapFrameEncode_Release(frame);
  if (encoder)
    IWICBitmapEncoder_Release(encoder);
  if (stream)
    IWICStream_Release(stream);
  if (factory)
    IWICImagingFactory_Release(factory);
  if (uninitializeCom)
    CoUninitialize();
  return success;
}

BOOL captureImageLoad(const char *utf8Path, CaptureImage *image) {
  if (image == NULL)
    return FALSE;
  WCHAR path[MAX_PATH];
  if (!utf8ToWidePath(utf8Path, path))
    return FALSE;

  HRESULT comResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  BOOL uninitializeCom = SUCCEEDED(comResult);
  if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE)
    return FALSE;

  BOOL success = FALSE;
  IWICImagingFactory *factory = NULL;
  IWICBitmapDecoder *decoder = NULL;
  IWICBitmapFrameDecode *frame = NULL;
  IWICFormatConverter *converter = NULL;
  UINT width = 0, height = 0;
  CaptureImage loaded = {0};

  if (FAILED(createWicFactory(&factory)))
    goto cleanup;
  if (FAILED(IWICImagingFactory_CreateDecoderFromFilename(
          factory, path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
          &decoder)))
    goto cleanup;
  if (FAILED(IWICBitmapDecoder_GetFrame(decoder, 0, &frame)))
    goto cleanup;
  if (FAILED(IWICBitmapFrameDecode_GetSize(frame, &width, &height)))
    goto cleanup;
  if (!captureImageCreate(&loaded, width, height))
    goto cleanup;
  if (FAILED(IWICImagingFactory_CreateFormatConverter(factory, &converter)))
    goto cleanup;
  if (FAILED(IWICFormatConverter_Initialize(
          converter, (IWICBitmapSource *)frame, &GUID_WICPixelFormat32bppBGRA,
          WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom)))
    goto cleanup;
  if (loaded.dataSize > UINT_MAX)
    goto cleanup;
  if (FAILED(IWICBitmapSource_CopyPixels((IWICBitmapSource *)converter, NULL,
                                         loaded.stride, (UINT)loaded.dataSize,
                                         loaded.pixels)))
    goto cleanup;

  captureImageFree(image);
  *image = loaded;
  ZeroMemory(&loaded, sizeof(loaded));
  success = TRUE;

cleanup:
  captureImageFree(&loaded);
  if (converter)
    IWICFormatConverter_Release(converter);
  if (frame)
    IWICBitmapFrameDecode_Release(frame);
  if (decoder)
    IWICBitmapDecoder_Release(decoder);
  if (factory)
    IWICImagingFactory_Release(factory);
  if (uninitializeCom)
    CoUninitialize();
  return success;
}
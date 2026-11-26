#include "tesseractOcrEngine.hh"
#include <tesseract/baseapi.h>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>

struct TesseractOcrEngine {
  tesseract::TessBaseAPI api;
  std::mutex mutex;
  bool initialized = false;
};
static bool convertPageSegmentationMode(TesseractOcrPageSegmentationMode mode,
                                        tesseract::PageSegMode *tesseractMode) {
  if (tesseractMode == nullptr) {
    return false;
  }

  switch (mode) {
  case TESSERACT_OCR_PSM_AUTO:
    *tesseractMode = tesseract::PSM_AUTO;
    return true;

  case TESSERACT_OCR_PSM_SINGLE_BLOCK:
    *tesseractMode = tesseract::PSM_SINGLE_BLOCK;
    return true;

  case TESSERACT_OCR_PSM_SINGLE_LINE:
    *tesseractMode = tesseract::PSM_SINGLE_LINE;
    return true;

  case TESSERACT_OCR_PSM_SINGLE_WORD:
    *tesseractMode = tesseract::PSM_SINGLE_WORD;
    return true;

  case TESSERACT_OCR_PSM_SPARSE_TEXT:
    *tesseractMode = tesseract::PSM_SPARSE_TEXT;
    return true;

  default:
    return false;
  }
}
VTStatus tesseractOcrEngineInitialize(TesseractOcrEngine **engine,
                                      const char *tessdataPath,
                                      const char *language) {
  if (engine == nullptr || *engine != nullptr) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  try {
    std::unique_ptr<TesseractOcrEngine> instance(new (std::nothrow)
                                                     TesseractOcrEngine());

    if (!instance) {
      return VT_STATUS_OUT_OF_MEMORY;
    }

    const char *dataPath = (tessdataPath != nullptr && tessdataPath[0] != '\0')
                               ? tessdataPath
                               : nullptr;
    const char *languageCode =
        (language != nullptr && language[0] != '\0') ? language : "eng";

    if (instance->api.Init(dataPath, languageCode) != 0) {
      return VT_STATUS_IO_ERROR;
    }

    instance->initialized = true;
    instance->api.SetPageSegMode(tesseract::PSM_AUTO);
    *engine = instance.release();

    return VT_STATUS_OK;
  } catch (const std::bad_alloc &) {
    return VT_STATUS_OUT_OF_MEMORY;
  } catch (...) {
    return VT_STATUS_IO_ERROR;
  }
}
void tesseractOcrEngineDestroy(TesseractOcrEngine **engine) {
  if (engine == nullptr || *engine == nullptr) {
    return;
  }

  TesseractOcrEngine *instance = *engine;
  *engine = nullptr;

  try {
    std::lock_guard<std::mutex> lock(instance->mutex);

    if (instance->initialized) {
      instance->api.End();
      instance->initialized = false;
    }
  } catch (...) {
  }

  delete instance;
}
VTStatus tesseractOcrEngineSetPageSegmentationMode(
    TesseractOcrEngine *engine, TesseractOcrPageSegmentationMode mode) {
  if (engine == nullptr || !engine->initialized) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  tesseract::PageSegMode tesseractMode;

  if (!convertPageSegmentationMode(mode, &tesseractMode)) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  try {
    std::lock_guard<std::mutex> lock(engine->mutex);

    if (!engine->initialized) {
      return VT_STATUS_INVALID_ARGUMENT;
    }

    engine->api.SetPageSegMode(tesseractMode);

    return VT_STATUS_OK;
  } catch (...) {
    return VT_STATUS_IO_ERROR;
  }
}
VTStatus tesseractOcrEngineCallback(const VTImage *image, VTRect region,
                                    char *utf8Output, size_t outputCapacity,
                                    void *userData) {
  if (utf8Output != nullptr && outputCapacity > 0) {
    utf8Output[0] = '\0';
  }

  if (image == nullptr || utf8Output == nullptr || outputCapacity == 0 ||
      userData == nullptr) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  TesseractOcrEngine *engine = static_cast<TesseractOcrEngine *>(userData);

  if (engine == nullptr || !engine->initialized) {
    return VT_STATUS_INVALID_ARGUMENT;
  }
  if (!vtImageIsValid(image)) {
    return VT_STATUS_INVALID_ARGUMENT;
  }
  if (!vtRectIsValid(region) || region.x < 0 || region.y < 0 ||
      region.x > image->width - region.width ||
      region.y > image->height - region.height) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  try {
    std::lock_guard<std::mutex> lock(engine->mutex);

    if (!engine->initialized) {
      return VT_STATUS_INVALID_ARGUMENT;
    }

    engine->api.SetImage(image->pixels, image->width, image->height, 3,
                         image->stride);
    engine->api.SetRectangle(region.x, region.y, region.width, region.height);

    if (engine->api.Recognize(nullptr) != 0) {
      return VT_STATUS_IO_ERROR;
    }

    std::unique_ptr<char[]> recognizedText(engine->api.GetUTF8Text());

    if (!recognizedText) {
      return VT_STATUS_IO_ERROR;
    }

    const size_t textLength = std::strlen(recognizedText.get());

    // Never silently truncate recognized text.
    if (textLength >= outputCapacity) {
      return VT_STATUS_INVALID_ARGUMENT;
    }

    // Copy the text, including its null terminator.
    std::memcpy(utf8Output, recognizedText.get(), textLength + 1);

    return VT_STATUS_OK;
  } catch (const std::bad_alloc &) {
    return VT_STATUS_OUT_OF_MEMORY;
  } catch (...) {
    return VT_STATUS_IO_ERROR;
  }
}
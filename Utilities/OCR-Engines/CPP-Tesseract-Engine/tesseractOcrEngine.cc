#include "tesseractOcrEngine.hh"

#include <tesseract/baseapi.h>
#include <tesseract/resultiterator.h>

#include <cmath>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>

struct TesseractOcrEngine {
  tesseract::TessBaseAPI api;
  std::mutex mutex;
  bool initalized = false;
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
static bool isValidRecognitionRegion(const VTImage *image, VTRect region) {
  return vtImageIsValid(image) && vtRectIsValid(region) && region.x >= 0 &&
         region.y >= 0 && region.x <= image->width - region.width &&
         region.y <= image->height - region.height;
}
static VTStatus addTesseractElement(tesseract::ResultIterator &iterator,
                                    tesseract::PageIteratorLevel level,
                                    VTOcrElementType type, VTOcrResult *result,
                                    size_t parentIndex, size_t *elementIndex) {
  std::unique_ptr<char[]> text(iterator.GetUTF8Text(level));
  if (!text)
    return VT_STATUS_IO_ERROR;

  int left;
  int top;
  int right;
  int bottom;

  if (!iterator.BoundingBox(level, &left, &top, &right, &bottom))
    return VT_STATUS_NOT_FOUND;
  if (right <= left || bottom <= top)
    return VT_STATUS_NOT_FOUND;

  float confidence = iterator.Confidence(level);
  if (!std::isfinite(confidence) || confidence < 0.0f || confidence > 100.0f)
    confidence = VT_OCR_CONFIDENCE_UNAVAILABLE;

  VTRect bounds = {left, top, right - left, bottom - top};

  return vtOcrResultAdd(result, type, bounds, confidence, text.get(),
                        parentIndex, elementIndex);
}
VTStatus tesseractOcrEngineInitialize(TesseractOcrEngine **engine,
                                      const char *tessdataPath,
                                      const char *language) {
  if (engine == nullptr || *engine != nullptr)
    return VT_STATUS_INVALID_ARGUMENT;

  try {
    std::unique_ptr<TesseractOcrEngine> instance(new (std::nothrow)
                                                     TesseractOcrEngine());
    if (!instance)
      return VT_STATUS_OUT_OF_MEMORY;

    const char *dataPath = (tessdataPath != nullptr && tessdataPath[0] != '\0')
                               ? tessdataPath
                               : nullptr;
    const char *languageCode =
        (language != nullptr && language[0] != '\0') ? language : "eng";

    if (instance->api.Init(dataPath, languageCode) != 0)
      return VT_STATUS_IO_ERROR;

    instance->initalized = true;
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
  if (engine == nullptr || *engine == nullptr)
    return;

  TesseractOcrEngine *instance = *engine;
  *engine = nullptr;

  try {
    std::lock_guard<std::mutex> lock(instance->mutex);

    if (instance->initalized) {
      instance->api.End();
      instance->initalized = false;
    }
  } catch (...) {
  }

  delete instance;
}
VTStatus tesseractOcrEngineSetPageSegmentationMode(
    TesseractOcrEngine *engine, TesseractOcrPageSegmentationMode mode) {
  if (engine == nullptr)
    return VT_STATUS_INVALID_ARGUMENT;

  tesseract::PageSegMode tesseractMode;
  if (!convertPageSegmentationMode(mode, &tesseractMode))
    return VT_STATUS_INVALID_ARGUMENT;

  try {
    std::lock_guard<std::mutex> lock(engine->mutex);
    if (!engine->initalized)
      return VT_STATUS_INVALID_ARGUMENT;

    engine->api.SetPageSegMode(tesseractMode);

    return VT_STATUS_OK;
  } catch (...) {
    return VT_STATUS_IO_ERROR;
  }
}

VTStatus tesseractOcrEngineCallback(const VTImage *image, VTRect region,
                                    char *utf8Output, size_t outputCapacity,
                                    void *userData) {
  if (utf8Output != nullptr && outputCapacity > 0)
    utf8Output[0] = '\0';
  if (utf8Output == nullptr || outputCapacity == 0 || userData == nullptr ||
      !isValidRecognitionRegion(image, region))
    return VT_STATUS_INVALID_ARGUMENT;

  TesseractOcrEngine *engine = static_cast<TesseractOcrEngine *>(userData);

  try {
    std::lock_guard<std::mutex> lock(engine->mutex);
    if (!engine->initalized)
      return VT_STATUS_INVALID_ARGUMENT;

    engine->api.SetImage(image->pixels, image->width, image->height, 3,
                         image->stride);
    engine->api.SetRectangle(region.x, region.y, region.width, region.height);
    if (engine->api.Recognize(nullptr) != 0)
      return VT_STATUS_IO_ERROR;

    std::unique_ptr<char[]> recognizedText(engine->api.GetUTF8Text());
    if (!recognizedText)
      return VT_STATUS_IO_ERROR;

    size_t textLength = std::strlen(recognizedText.get());
    if (textLength >= outputCapacity)
      return VT_STATUS_INVALID_ARGUMENT;

    std::memcpy(utf8Output, recognizedText.get(), textLength + 1);

    return VT_STATUS_OK;
  } catch (const std::bad_alloc &) {
    return VT_STATUS_OUT_OF_MEMORY;
  } catch (...) {
    return VT_STATUS_IO_ERROR;
  }
}
VTStatus tesseractOcrEngineStructuredCallback(const VTImage *image,
                                              VTRect region,
                                              VTOcrResult *result,
                                              void *userData) {
  if (result == nullptr || userData == nullptr ||
      !isValidRecognitionRegion(image, region) ||
      result->count > result->capacity ||
      (result->count > 0 && result->elements == nullptr)) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  TesseractOcrEngine *engine = static_cast<TesseractOcrEngine *>(userData);

  try {
    std::lock_guard<std::mutex> lock(engine->mutex);
    if (!engine->initalized) {
      return VT_STATUS_INVALID_ARGUMENT;
    }

    engine->api.SetImage(image->pixels, image->width, image->height, 3,
                         image->stride);
    engine->api.SetRectangle(region.x, region.y, region.width, region.height);
    if (engine->api.Recognize(nullptr) != 0) {
      return VT_STATUS_IO_ERROR;
    }

    std::unique_ptr<tesseract::ResultIterator> iterator(
        engine->api.GetIterator());
    if (!iterator) {
      return VT_STATUS_NOT_FOUND;
    }

    size_t blockIndex = VT_OCR_NO_PARENT;
    size_t paragraphIndex = VT_OCR_NO_PARENT;
    size_t lineIndex = VT_OCR_NO_PARENT;
    size_t wordIndex = VT_OCR_NO_PARENT;

    do {
      VTStatus status;
      if (iterator->IsAtBeginningOf(tesseract::RIL_BLOCK)) {
        status = addTesseractElement(*iterator, tesseract::RIL_BLOCK,
                                     VT_OCR_ELEMENT_BLOCK, result,
                                     VT_OCR_NO_PARENT, &blockIndex);

        if (status != VT_STATUS_OK) {
          return status;
        }
      }
      if (iterator->IsAtBeginningOf(tesseract::RIL_PARA)) {
        status = addTesseractElement(*iterator, tesseract::RIL_PARA,
                                     VT_OCR_ELEMENT_PARAGRAPH, result,
                                     blockIndex, &paragraphIndex);
        if (status != VT_STATUS_OK) {
          return status;
        }
      }
      if (iterator->IsAtBeginningOf(tesseract::RIL_TEXTLINE)) {
        status = addTesseractElement(*iterator, tesseract::RIL_TEXTLINE,
                                     VT_OCR_ELEMENT_LINE, result,
                                     paragraphIndex, &lineIndex);
        if (status != VT_STATUS_OK) {
          return status;
        }
      }
      if (iterator->IsAtBeginningOf(tesseract::RIL_WORD)) {
        status = addTesseractElement(*iterator, tesseract::RIL_WORD,
                                     VT_OCR_ELEMENT_WORD, result, lineIndex,
                                     &wordIndex);
        if (status != VT_STATUS_OK) {
          return status;
        }
      }

      status = addTesseractElement(*iterator, tesseract::RIL_SYMBOL,
                                   VT_OCR_ELEMENT_SYMBOL, result, wordIndex,
                                   nullptr);
      if (status != VT_STATUS_OK) {
        return status;
      }

    } while (iterator->Next(tesseract::RIL_SYMBOL));
    return VT_STATUS_OK;

  } catch (const std::bad_alloc &) {
    return VT_STATUS_OUT_OF_MEMORY;
  } catch (...) {
    return VT_STATUS_IO_ERROR;
  }
}
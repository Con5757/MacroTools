#pragma once

#include "../../../src/VisualTools (Pattern-Recognition)/textRecognitionTool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TesseractOcrEngine TesseractOcrEngine;
typedef enum TesseractOcrPageSegmentationMode {
  TESSERACT_OCR_PSM_AUTO = 0,
  TESSERACT_OCR_PSM_SINGLE_BLOCK,
  TESSERACT_OCR_PSM_SINGLE_LINE,
  TESSERACT_OCR_PSM_SINGLE_WORD,
  TESSERACT_OCR_PSM_SPARSE_TEXT
} TesseractOcrPageSegmentationMode;

VTStatus tesseractOcrEngineInitialize(TesseractOcrEngine **engine,
                                      const char *tessdataPath,
                                      const char *language);

void tesseractOcrEngineDestroy(TesseractOcrEngine **engine);
VTStatus tesseractOcrEngineSetPageSegmentationMode(
    TesseractOcrEngine *engine, TesseractOcrPageSegmentationMode mode);
VTStatus tesseractOcrEngineCallback(const VTImage *image, VTRect region,
                                    char *utf8Output, size_t outputCapacity,
                                    void *userData);
VTStatus tesseractOcrEngineStructuredCallback(const VTImage *image,
                                              VTRect region,
                                              VTOcrResult *result,
                                              void *userData);

#ifdef __cplusplus
}
#endif
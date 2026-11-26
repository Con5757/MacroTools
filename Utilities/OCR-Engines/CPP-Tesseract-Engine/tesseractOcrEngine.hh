#pragma once

#include "../../../src/VisualTools (Pattern-Recognition)/textRecognitionTool.h"

#ifdef __cplusplus
extern "C" {
#endif

// engine context.
typedef struct TesseractOcrEngine TesseractOcrEngine;
// modes.
typedef enum TesseractOcrPageSegmentationMode {
  TESSERACT_OCR_PSM_AUTO = 0,
  TESSERACT_OCR_PSM_SINGLE_BLOCK,
  TESSERACT_OCR_PSM_SINGLE_LINE,
  TESSERACT_OCR_PSM_SINGLE_WORD,
  TESSERACT_OCR_PSM_SPARSE_TEXT
} TesseractOcrPageSegmentationMode;

// init the engine
VTStatus tesseractOcrEngineInitialize(TesseractOcrEngine **engine,
                                      const char *tessdataPath,
                                      const char *language);
// release its resources
void tesseractOcrEngineDestroy(TesseractOcrEngine **engine);
// page segment config
VTStatus tesseractOcrEngineSetPageSegmentationMode(
    TesseractOcrEngine *engine, TesseractOcrPageSegmentationMode mode);
// Recognition callback compatible with VTOcrEngine.
VTStatus tesseractOcrEngineCallback(const VTImage *image, VTRect region,
                                    char *utf8Output, size_t outputCapacity,
                                    void *userData);

#ifdef __cplusplus
}
#endif
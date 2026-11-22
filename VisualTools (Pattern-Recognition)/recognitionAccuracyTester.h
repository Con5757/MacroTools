#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t truePositives;
  size_t falsePositives;
  size_t falseNegatives;
  double precision;
  double recall;
  double f1Score;
} VTAccuracyReport;

VTStatus vtRecognitionMeasureAccuracy(const VTRect *predictions,
                                      size_t predictionCount,
                                      const VTRect *groundTruth,
                                      size_t truthCount, double minimumIoU,
                                      VTAccuracyReport *report);
                                      
#ifdef __cplusplus
}
#endif
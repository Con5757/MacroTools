#pragma once
#include "common/captureWic.h"

BOOL captureFormatConvertFile(const char *inputPathUtf8,
                              const char *outputPathUtf8,
                              CaptureImageFormat outputFormat);
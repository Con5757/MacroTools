#pragma once
#include "macroToolsCommon.h"

typedef struct {
  size_t errors;
  size_t warnings;
  size_t first_error_index;
  char first_error[128];
} MacroValidationReport;

bool macroValidatorValidate(const MacroSequence *sequence,
                            MacroValidationReport *report);
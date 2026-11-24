#pragma once
#include "logicToolsCommon.h"

typedef enum {
  LA_EXPR_OK,
  LA_EXPR_INVALID_ARGUMENT,
  LA_EXPR_SYNTAX_ERROR,
  LA_EXPR_UNKNOWN_VARIABLE,
  LA_EXPR_DIVIDE_BY_ZERO,
  LA_EXPR_NONFINITE_RESULT
} LAExpressionStatus;
typedef struct {
  LAExpressionStatus status;
  size_t errorOffset;
} LAExpressionResult;
typedef struct {
  const char *s;
  size_t pos;
  LAValueResolver resolver;
  void *ctx;
  LAExpressionStatus status;
} Parser;

LAExpressionStatus laExpressionEvaluate(const char *expression,
                                        LAValueResolver resolver, void *context,
                                        double *result, size_t *errorOffset);
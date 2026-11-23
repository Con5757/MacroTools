#pragma once
#include "logicToolsCommon.h"

typedef enum {
  LA_COMPARE_EQUAL,
  LA_COMPARE_NOT_EQUAL,
  LA_COMPARE_LESS,
  LA_COMPARE_LESS_EQUAL,
  LA_COMPARE_GREATER,
  LA_COMPARE_GREATER_EQUAL
} LACompareOperator;

bool laConditionCompare(double left, LACompareOperator operation, double right,
                        double epsilon);
bool laConditionAnd(bool left, bool right);
bool laConditionOr(bool left, bool right);
bool laConditionNot(bool value);
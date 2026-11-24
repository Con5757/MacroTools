#include "expressionEvaluator.h"
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static void skip(Parser *p) {
  while (isspace((unsigned char)p->s[p->pos]))
    p->pos++;
}
static double parseExpr(Parser *p);
static double parsePrimary(Parser *p) {
  skip(p);
  char c = p->s[p->pos];
  if (c == '+' || c == '-') {
    p->pos++;
    double v = parsePrimary(p);
    return c == '-' ? -v : v;
  }
  if (c == '(') {
    p->pos++;
    double v = parseExpr(p);
    skip(p);
    if (p->s[p->pos] != ')') {
      p->status = LA_EXPR_SYNTAX_ERROR;
      return 0;
    }
    p->pos++;
    return v;
  }
  if (isdigit((unsigned char)c) || c == '.') {
    char *end = NULL;
    double v = strtod(p->s + p->pos, &end);
    if (end == p->s + p->pos) {
      p->status = LA_EXPR_SYNTAX_ERROR;
      return 0;
    }
    p->pos = (size_t)(end - p->s);
    return v;
  }
  if (isalpha((unsigned char)c) || c == '_') {
    char name[LA_NAME_CAPACITY];
    size_t n = 0;
    while (isalnum((unsigned char)p->s[p->pos]) || p->s[p->pos] == '_') {
      if (n + 1 >= sizeof(name)) {
        p->status = LA_EXPR_SYNTAX_ERROR;
        return 0;
      }
      name[n++] = p->s[p->pos];
      p->pos++;
    }
    name[n] = '\0';
    if (strcmp(name, "pi") == 0)
      return 3.14159265358979323846;
    if (strcmp(name, "e") == 0)
      return 2.71828182845904523536;
    double v = 0;
    if (!p->resolver || !p->resolver(p->ctx, name, &v)) {
      p->status = LA_EXPR_UNKNOWN_VARIABLE;
      return 0;
    }
    return v;
  }
  p->status = LA_EXPR_SYNTAX_ERROR;
  return 0;
}
static double parsePower(Parser *p) {
  double v = parsePrimary(p);
  skip(p);
  if (p->s[p->pos] == '^') {
    p->pos++;
    double rhs = parsePower(p);
    v = pow(v, rhs);
  }
  return v;
}
static double parseTerm(Parser *p) {
  double v = parsePower(p);
  for (;;) {
    skip(p);
    char op = p->s[p->pos];
    if (op != '*' && op != '/' && op != '%')
      break;
    p->pos++;
    double rhs = parsePower(p);
    if ((op == '/' || op == '%') && rhs == 0.0) {
      p->status = LA_EXPR_DIVIDE_BY_ZERO;
      return 0;
    }
    if (op == '*')
      v *= rhs;
    else if (op == '/')
      v /= rhs;
    else
      v = fmod(v, rhs);
  }
  return v;
}
static double parseExpr(Parser *p) {
  double v = parseTerm(p);
  for (;;) {
    skip(p);
    char op = p->s[p->pos];
    if (op != '+' && op != '-')
      break;
    p->pos++;
    double rhs = parseTerm(p);
    v = op == '+' ? v + rhs : v - rhs;
  }
  return v;
}
LAExpressionStatus laExpressionEvaluate(const char *expr,
                                        LAValueResolver resolver, void *ctx,
                                        double *out, size_t *errorOffset) {
  if (errorOffset)
    *errorOffset = 0;
  if (!expr || !out)
    return LA_EXPR_INVALID_ARGUMENT;
  Parser p = {expr, 0, resolver, ctx, LA_EXPR_OK};
  double v = parseExpr(&p);
  skip(&p);
  if (p.status == LA_EXPR_OK && p.s[p.pos] != '\0')
    p.status = LA_EXPR_SYNTAX_ERROR;
  if (p.status == LA_EXPR_OK && !isfinite(v))
    p.status = LA_EXPR_NONFINITE_RESULT;
  if (errorOffset)
    *errorOffset = p.pos;
  if (p.status == LA_EXPR_OK)
    *out = v;
  return p.status;
}
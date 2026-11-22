#include "macroValidator.h"
#include <math.h>
#include <string.h>

static void addError(MacroValidationReport *r, size_t i, const char *msg) {
  if (r->errors == 0) {
    r->first_error_index = i;
    strncpy(r->first_error, msg, sizeof(r->first_error) - 1);
    r->first_error[sizeof(r->first_error) - 1] = '\0';
  }
  r->errors++;
}
bool macroValidatorValidate(const MacroSequence *s, MacroValidationReport *r) {
  if (!s || !r)
    return false;
  memset(r, 0, sizeof(*r));
  r->first_error_index = SIZE_MAX;
  if (s->name[0] == '\0')
    addError(r, 0, "Macro name is empty");
  if (s->count > 0 && s->steps == NULL) {
    addError(r, 0, "Step storage is missing");
    return false;
  }
  for (size_t i = 0; i < s->count; i++) {
    const MacroStep *p = &s->steps[i];
    if (p->type < MACRO_STEP_KEY_DOWN || p->type > MACRO_STEP_CHECKPOINT)
      addError(r, i, "Unknown step type");
    if (memchr(p->text, '\0', sizeof(p->text)) == NULL)
      addError(r, i, "Step text is not terminated");
    if (!isfinite(p->number))
      addError(r, i, "Step numeric value is not finite");
    if (p->type == MACRO_STEP_JUMP && p->target_step >= s->count)
      addError(r, i, "Jump target is outside the macro");
    if (p->type == MACRO_STEP_CONDITION) {
      if (p->code < MACRO_CONDITION_EQUAL || p->code > MACRO_CONDITION_FALSEY)
        addError(r, i, "Condition operator is invalid");
      if (p->target_step != MACRO_NO_TARGET && p->target_step >= s->count)
        addError(r, i, "True branch target is outside the macro");
      if (p->alternate_step != MACRO_NO_TARGET && p->alternate_step >= s->count)
        addError(r, i, "False branch target is outside the macro");
      if (p->text[0] == '\0')
        addError(r, i, "Condition variable name is empty");
    }
    if (p->type == MACRO_STEP_SET_VARIABLE && p->text[0] == '\0')
      addError(r, i, "Variable name is empty");
    for (size_t j = 0; j < i; j++)
      if (s->steps[j].id == p->id && p->id != 0) {
        addError(r, i, "Duplicate step identifier");
        break;
      }
  }
  return r->errors == 0;
}
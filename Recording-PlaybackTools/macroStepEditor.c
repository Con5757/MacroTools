#include "macroStepEditor.h"
#include <string.h>

bool macroStepEditorSetDelay(MacroSequence *s, size_t i, uint64_t d) {
  MacroStep *p = macroSequenceGet(s, i);
  if (!p)
    return false;
  p->delay_ms = d;
  return true;
}
bool macroStepEditorSetMousePosition(MacroSequence *s, size_t i, int32_t x,
                                     int32_t y) {
  MacroStep *p = macroSequenceGet(s, i);
  if (!p || p->type != MACRO_STEP_MOUSE_MOVE)
    return false;
  p->x = x;
  p->y = y;
  return true;
}
bool macroStepEditorSetCodeValue(MacroSequence *s, size_t i, int32_t c,
                                 int32_t v) {
  MacroStep *p = macroSequenceGet(s, i);
  if (!p)
    return false;
  p->code = c;
  p->value = v;
  return true;
}
bool macroStepEditorSetText(MacroSequence *s, size_t i, const char *text) {
  MacroStep *p = macroSequenceGet(s, i);
  if (!p || text == NULL || strlen(text) >= MACRO_TEXT_CAPACITY)
    return false;
  strcpy(p->text, text);
  return true;
}
bool macroStepEditorSetBranch(MacroSequence *s, size_t i, uint32_t t,
                              uint32_t f) {
  MacroStep *p = macroSequenceGet(s, i);
  if (!p || (p->type != MACRO_STEP_CONDITION && p->type != MACRO_STEP_JUMP))
    return false;
  p->target_step = t;
  p->alternate_step = f;
  return true;
}
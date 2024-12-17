/* escape.h */

#ifndef ESCAPE_H
#define ESCAPE_H
#include "absyn.h"
#include "env.h"
#include "util.h"

void Esc_findEscape(A_exp exp);
U_boolList genEscapeList(Ty_tyList formals);

#endif //ESCAPE_H

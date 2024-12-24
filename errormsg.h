#ifndef ERRORMSG_H
#define ERRORMSG_H
#include "absyn.h"
#include "util.h"

extern bool EM_anyErrors;

_Noreturn void EM_error(A_pos, string, ...);
void EM_reset(string filename);
#endif

#ifndef ERRORMSG_H
#define ERRORMSG_H
#include "absyn.h"
#include "util.h"

#if defined(__cplusplus)
[[noreturn]] void EM_error(A_pos, string, ...);
#else
_Noreturn void EM_error(A_pos, string, ...);
#endif
void EM_reset(string filename);
#endif

#ifndef ERRORMSG_H
#define ERRORMSG_H
#include "util.h"
#include "absyn.h"

extern bool EM_anyErrors;

void EM_error(A_pos, string,...);
void EM_reset(string filename);
#endif

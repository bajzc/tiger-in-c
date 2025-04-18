/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "prabsyn.h"
#include "errormsg.h"
#include "util.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname;
   return abstract syntax data structure */
A_exp parse(string fname) {
  EM_reset(fname);
  if (yyparse() == 0) /* parsing worked */
    return absyn_root;
  return NULL;
}
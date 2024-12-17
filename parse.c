/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "errormsg.h"
#include "escape.h"
#include "parse.h"
#include "semant.h"
#include "util.h"
#include "prabsyn.h"

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

int main(int argc, char **argv) {
  if (argc != 2) {
    debug("usage: a.out filename\n");
    exit(1);
  }
  debug("call parse()\n");
  if (parse(argv[1]) == NULL) {
    printf("parser error\n");
    exit(1);
  }
  fflush(stdout);
  debug("call Esc_findEscape()\n");
  // Esc_findEscape(absyn_root);

#if DEBUG2
  debug("call pr_exp()\n");
  fflush(stderr);
  pr_exp(stdout, absyn_root, 0);
  fflush(stdout);
#endif

  debug("call SEM_transProg()\n");
  SEM_transProg(absyn_root);
  return 0;
}

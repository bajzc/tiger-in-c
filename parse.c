/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "prabsyn.h"
#include "types.h"
#include "semant.h"
#include "parse.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname) 
{EM_reset(fname);
 if (yyparse() == 0) /* parsing worked */
   return absyn_root;
 else return NULL;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: a.out filename\n");
    exit(1);
  }
  fprintf(stderr,"call parse\n");
  if (parse(argv[1]) == NULL) {
    fprintf(stderr, "parser error\n");
    exit(1);
  }
  /* fprintf(stderr,"call pr_exp\n"); */
  /* pr_exp(stdout, absyn_root, 0); */
	/* fflush(stdout); */
  fprintf(stderr, "\ncall SEM_transProg\n");
  SEM_transProg(absyn_root);
  return 0;
}

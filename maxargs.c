#include "util.h"
#include "slp.h"
#include "prog1.h"
#include <stdio.h>
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int max = 0;

int maxargs(A_stm);
int traverse_exp(A_exp exp) {
  switch (exp->kind) {
  case A_idExp:
  case A_numExp:
  case A_opExp:
    return 1;
  case A_eseqExp:
    maxargs(exp->u.eseq.stm);
    return 1;
  };
}

int maxargs(A_stm stm) {
  if (stm->kind == A_printStm) {
    A_expList print_exp = stm->u.print.exps;
    if (print_exp->kind == A_pairExpList) {
      int count = 0;
      count += traverse_exp(print_exp->u.pair.head);
      while (print_exp->u.pair.tail->kind != A_lastExpList) {
        print_exp = print_exp->u.pair.tail;
        count += traverse_exp(print_exp->u.pair.head);
      }
      count += traverse_exp(print_exp->u.last);
      max = MAX(count, max);
    } else if (print_exp->kind == A_lastExpList) {
      assert(1);
    } else {
      assert(1);
    }
  } else if (stm->kind == A_compoundStm) {
    maxargs(stm->u.compound.stm1);
    maxargs(stm->u.compound.stm2);
  } else if (stm->kind == A_assignStm) {
    traverse_exp(stm->u.assign.exp);
  }
  return max;
}

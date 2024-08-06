#include "prog1.h"
#include "slp.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct table *Table_;
struct table {
  string id;
  int value;
  Table_ tail;
};
struct IntAndTable {
  int i;
  Table_ t;
};
struct IntAndTable interpExp(A_exp e, Table_ t);
Table_ interpStm(A_stm s, Table_ t);

Table_ Table(string id, int value, struct table *tail) {
  Table_ t = malloc(sizeof(*t));
  t->id = id;
  t->value = value;
  t->tail = tail;
  return t;
}

int lookupTable(string id, struct table *T) {
  struct table *p = T;
  while (p) {
    if (strcmp((char *)p->id, (char *)id) == 0) {
      return p->value;
    }
    p = p->tail;
  }
  assert(1);
  return 0;
}

Table_ interpStm(A_stm s, Table_ t) {
  struct IntAndTable it;
  A_expList exps;
  switch (s->kind) {
  case A_compoundStm:
    t = interpStm(s->u.compound.stm1, t);
    t = interpStm(s->u.compound.stm2, t);
    break;
  case A_assignStm:
    it = interpExp(s->u.assign.exp, t);
    t = Table(s->u.assign.id, it.i, t);
    break;
  case A_printStm:
    exps = s->u.print.exps;
    while (exps->kind != A_lastExpList) {
      it = interpExp(exps->u.pair.head, t);
      t = it.t;
      printf("%d ", it.i);
      exps = exps->u.pair.tail;
    }
    it = interpExp(exps->u.last, t);
    t = it.t;
    printf("%d\n", it.i);
    break;
  };
  return t;
}

struct IntAndTable interpExp(A_exp e, Table_ t) {
  struct IntAndTable it;
  int rval, lval;
  switch (e->kind) {
  case A_idExp:
    it.i = lookupTable(e->u.id, t);
    it.t = t;
    break;
  case A_numExp:
    it.i = e->u.num;
    it.t = t;
    break;
  case A_opExp:
    rval = interpExp(e->u.op.right, t).i;
    lval = interpExp(e->u.op.left, t).i;
    switch (e->u.op.oper) {
    case A_plus:
      it.i = lval + rval;
      break;
    case A_minus:
      it.i = lval - rval;
      break;
    case A_times:
      it.i = lval * rval;
      break;
    case A_div:
      it.i = lval / rval;
      break;
    default:
      assert(1);
    }
    it.t = t;
    break;
  case A_eseqExp:
    interpStm(e->u.eseq.stm, t);
    it = interpExp(e->u.eseq.exp, t);
  };
  return it;
}

int main() {
  interpStm(prog(), NULL);
  return 0;
}

#ifndef CANON_H
#define CANON_H

#include "temp.h"
#include "translate.h"
#include "util.h"

typedef struct C_stmListList_ *C_stmListList;
struct C_block {
  C_stmListList stmLists;
  Temp_label label;
};
struct C_stmListList_ {
  T_stmList head;
  C_stmListList tail;
};

T_stmList C_linearize(T_stm stm);
struct C_block C_basicBlocks(T_stmList stmList);
T_stmList C_traceSchedule(struct C_block b);

static bool isNop(T_stm x);
static bool commute(T_stm x, T_stm y);

typedef struct expRefList_ *expRefList;
struct expRefList_ {
  T_exp *head;
  expRefList tail;
};
struct stmExp {
  T_stm s;
  T_exp e;
};

static T_stm reorder(expRefList rlist);
static T_stm do_stm(T_stm stm);
static struct stmExp do_exp(T_exp exp);

static expRefList ExpRefList(T_exp *, expRefList);
static struct stmExp StmExp(T_stm, T_exp);
static T_stm seq(T_stm, T_stm);

#endif // CANON_H

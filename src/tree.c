#include <stdio.h>

#include "tree.h"

#include "frame.h"
#include "symbol.h"
#include "temp.h"
#include "util.h"

T_expList T_ExpList(T_exp head, T_expList tail) {
  T_expList p = (T_expList) checked_malloc(sizeof *p);
  p->head = head;
  p->tail = tail;
  return p;
}

T_stmList T_StmList(T_stm head, T_stmList tail) {
  T_stmList p = (T_stmList) checked_malloc(sizeof *p);
  p->head = head;
  p->tail = tail;
  return p;
}

T_stm T_Seq(T_stm left, T_stm right) {
  T_stm p = (T_stm) checked_malloc(sizeof *p);
  p->kind = T_SEQ;
  p->u.SEQ.left = left;
  p->u.SEQ.right = right;
  return p;
}

/**
 * Define the constant value of name `label` to be the current machine
 * code address. This is like a label definition in assembly language. The value
 * `NAME(label)` maybe the target of jumps, calls, etc.
 */
T_stm T_Label(Temp_label label) {
  T_stm p = (T_stm) checked_malloc(sizeof *p);
  p->kind = T_LABEL;
  p->u.LABEL = label;
  return p;
}

/**
 * @brief jump to exp, common use: T_Jump(l,Temp_LabelList(l,NULL));
 * @param exp destination address, may be a literal label, as in NAME(lab), or
 * it may be an address calculated by any other kind of expression
 * @param labels The list of labels labs specifies all the possible locations
 * that the expression e can evaluate to; this is necessary for dataflow
 * analysis later
 */
T_stm T_Jump(T_exp exp, Temp_labelList labels) {
  T_stm p = (T_stm) checked_malloc(sizeof *p);
  p->kind = T_JUMP;
  p->u.JUMP.exp = exp;
  p->u.JUMP.jumps = labels;
  return p;
}

T_stm T_Cjump(T_relOp op, T_exp left, T_exp right, Temp_label truee,
              Temp_label falsee) {
  T_stm p = (T_stm) checked_malloc(sizeof *p);
  p->kind = T_CJUMP;
  p->u.CJUMP.op = op;
  p->u.CJUMP.left = left;
  p->u.CJUMP.right = right;
  p->u.CJUMP.truee = truee;
  p->u.CJUMP.falsee = falsee;
  return p;
}

/**
 * Move(Temp t, e) Evaluate `e` and move it into temporary `t` Move(Mem(e1), e2)
 * Evaluate `e1`, yielding address `a`. Then evaluate `e2`, and store the result
 * into *wordSize* bytes of memory starting at `a`. */
T_stm T_Move(T_exp dst, T_exp src) {
  T_stm p = (T_stm) checked_malloc(sizeof *p);
  p->kind = T_MOVE;
  p->u.MOVE.dst = dst;
  p->u.MOVE.src = src;
  return p;
}

T_stm T_Exp(T_exp exp) {
  T_stm p = (T_stm) checked_malloc(sizeof *p);
  p->kind = T_EXP;
  p->u.EXP = exp;
  return p;
}

T_exp T_Binop(T_binOp op, T_exp left, T_exp right) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_BINOP;
  p->u.BINOP.op = op;
  p->u.BINOP.left = left;
  p->u.BINOP.right = right;
  return p;
}

/**
 * The contents of *wordSize* bytes of memory starting at address `exp`. Note
 * that when `MEM` is used as the left child of a `MOVE`, it means “store,” but
 * anywhere else it means “fetch.” */
T_exp T_Mem(T_exp exp) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_MEM;
  p->u.MEM = exp;
  return p;
}

T_exp T_Temp(Temp_temp temp) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_TEMP;
  p->u.TEMP = temp;
  return p;
}

/** The statement `stm` is evaluated for side effects, then `exp` is evaluated
 * for a result **/
T_exp T_Eseq(T_stm stm, T_exp exp) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_ESEQ;
  p->u.ESEQ.stm = stm;
  p->u.ESEQ.exp = exp;
  return p;
}

/**
 * @brief The symbolic constant name (corresponding to an assembly language
 * label)
 */
T_exp T_Name(Temp_label name) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_NAME;
  p->u.NAME = name;
  return p;
}

T_exp T_Const(int consti) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_CONST;
  p->u.CONST = consti;
  return p;
}

T_exp T_Call(T_exp fun, T_expList args) {
  T_exp p = (T_exp) checked_malloc(sizeof *p);
  p->kind = T_CALL;
  p->u.CALL.fun = fun;
  p->u.CALL.args = args;
  return p;
}

T_relOp T_notRel(T_relOp r) {
  switch (r) {
    case T_eq: return T_ne;
    case T_ne: return T_eq;
    case T_lt: return T_ge;
    case T_ge: return T_lt;
    case T_gt: return T_le;
    case T_le: return T_gt;
    case T_ult: return T_uge;
    case T_uge: return T_ult;
    case T_ule: return T_ugt;
    case T_ugt: return T_ule;
  }
  assert(0);
  return 0;
}

T_relOp T_commute(T_relOp r) {
  switch (r) {
    case T_eq: return T_eq;
    case T_ne: return T_ne;
    case T_lt: return T_gt;
    case T_ge: return T_le;
    case T_gt: return T_lt;
    case T_le: return T_ge;
    case T_ult: return T_ugt;
    case T_uge: return T_ule;
    case T_ule: return T_uge;
    case T_ugt: return T_ult;
  }
  assert(0);
  return 0;
}

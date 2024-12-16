#include "canon.h"

static bool isNop(T_stm x) {
  return x->kind == T_EXP && x->u.EXP->kind == T_CONST;
}
static bool commute(T_stm x, T_stm y) {
  return isNop(x) || y->kind == T_NAME || y->kind == T_CONST;
}

static expRefList ExpRefList(T_exp *head, expRefList tail) {
  const expRefList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}

static struct stmExp StmExp(T_stm s, T_exp e) {
  const struct stmExp r = {s, e};
  return r;
}

static expRefList get_call_rlist(T_exp exp) {
  if (exp->kind != T_CALL)
    return NULL;

  expRefList list_head = NULL;
  expRefList *list_tail = NULL;

  T_expList cur_expList = exp->u.CALL.args;

  while (cur_expList) {
    if (list_tail == NULL) {
      list_head = ExpRefList(&cur_expList->head, NULL);
      list_tail = &list_head;
    } else {
      *list_tail = ExpRefList(&cur_expList->head, *list_tail);
      list_tail = &(*list_tail)->tail;
    }

    cur_expList = cur_expList->tail;
  }

  return list_head;
}

T_stm reorder(const expRefList rlist) {
  T_stm result_head = NULL;
  T_stm *result_tail = NULL;

  expRefList cur = rlist;
  while (cur) {
    T_exp *e = cur->head;
    if ((*e)->kind == T_ESEQ) {
      // Pull inner ESEQs out and join sequences together

      const T_stm seq = do_stm((*e)->u.ESEQ.stm);
      if (result_tail == NULL) {
        result_head = seq;
        result_tail = &result_head;
      } else {
        *result_tail = T_Seq(*result_tail, seq);
        result_tail = &(*result_tail)->u.SEQ.right;
      }

      // Pull eseq.exp out
      *e = (*e)->u.ESEQ.exp;
    }
    cur = cur->tail;
  }
  return result_head;
}

static T_stm do_stm(const T_stm stm) {
  switch (stm->kind) {
    case T_SEQ:
      return seq(do_stm(stm->u.SEQ.left), do_stm(stm->u.SEQ.right));
    case T_JUMP:
      return seq(reorder(ExpRefList(&stm->u.JUMP.exp, NULL)), stm);
    case T_CJUMP:
      return seq(reorder(ExpRefList(&stm->u.CJUMP.left,
                                    ExpRefList(&stm->u.CJUMP.right, NULL))),
                 stm);
    case T_MOVE:
      if (stm->u.MOVE.dst->kind == T_TEMP && stm->u.MOVE.src->kind == T_CALL)
        return seq(reorder(get_call_rlist(stm->u.MOVE.src)), stm);
      if (stm->u.MOVE.dst->kind == T_TEMP)
        return seq(reorder(ExpRefList(&stm->u.MOVE.src, NULL)), stm);
      if (stm->u.MOVE.dst->kind == T_MEM)
        return seq(reorder(ExpRefList(&stm->u.MOVE.dst->u.MEM,
                                      ExpRefList(&stm->u.MOVE.src, NULL))),
                   stm);
      if (stm->u.MOVE.dst->kind == T_ESEQ) {
        T_stm s = stm->u.MOVE.dst->u.ESEQ.stm;
        stm->u.MOVE.dst = stm->u.MOVE.dst->u.ESEQ.exp;
        return do_stm(T_Seq(s, stm));
      }
    case T_EXP:
      if (stm->u.EXP->kind == T_CALL)
        return seq(reorder(get_call_rlist(stm->u.EXP)), stm);
      return seq(reorder(ExpRefList(&stm->u.EXP, NULL)), stm);
    default:
      return stm;
  }
}

static struct stmExp do_exp(const T_exp exp) {
  switch (exp->kind) {
    case T_BINOP:
      if (exp->u.BINOP.left->kind == T_CALL &&
          exp->u.BINOP.right->kind == T_CALL) {
        const T_exp t_new = T_Temp(Temp_newtemp());
        exp->u.BINOP.left = T_Eseq(T_Move(t_new, exp->u.BINOP.left), t_new);
        return do_exp(exp);
      }
      return StmExp(reorder(ExpRefList(&exp->u.BINOP.left,
                                       ExpRefList(&exp->u.BINOP.right, NULL))),
                    exp);
    case T_MEM:
      return StmExp(reorder(ExpRefList(&exp->u.MEM, NULL)), exp);
    case T_ESEQ: {
      struct stmExp x = do_exp(exp->u.ESEQ.exp);
      return StmExp(seq(do_stm(exp->u.ESEQ.stm), x.s), x.e);
    }
    case T_CALL:
      return StmExp(reorder(get_call_rlist(exp)), exp);
    default:
      return StmExp(reorder(NULL), exp);
  }
}

static T_stm seq(const T_stm x, const T_stm y) {
  if (isNop(x))
    return y;
  if (isNop(y))
    return x;
  return T_Seq(x, y);
}

static T_stmList linear(const T_stm s, const T_stmList ss) {
  if (s->kind == T_SEQ)
    return T_StmList(s->u.SEQ.left, linear(s->u.SEQ.right, ss));
  return T_StmList(s, ss);
}

T_stmList C_linearize(const T_stm stm) { return linear(do_stm(stm), NULL); }

#include <stdio.h>

#include "codegen.h"
#include "set.h"

#define L(h, t) Temp_TempList((Temp_temp) h, (Temp_tempList) t)
#define S(format, ...) snprintf(buf, 80, format, ##__VA_ARGS__)

static AS_instrList iList = NULL, last = NULL;

static void munchStm(T_stm s);
static Temp_temp munchExp(T_exp e);
static Temp_tempList munchArgs(int n, T_expList args);

static void emit(AS_instr inst) {
  if (last != NULL)
    last = last->tail = AS_InstrList(inst, NULL);
  else
    last = iList = AS_InstrList(inst, NULL);
}

static Temp_tempList munchArgs(int n, T_expList args) {
  if (args == NULL)
    return NULL;
  Temp_tempList right = munchArgs(n + 1, args->tail);

  Temp_temp r = munchExp(args->head);

  emit(AS_Oper("addi `d0, `s0, -4 # grow up stack", L(F_SP(), NULL),
               L(F_SP(), NULL), NULL));
  // not sure if we need to use AS_Move here. If this instruction is eliminated,
  // a word in stack is just wasted
  emit(AS_Oper("sw `s0, 0(`d0) # push onto stack", L(F_SP(), NULL), L(r, NULL),
               NULL));
  return L(r, right);
}

static void munchStm(T_stm s) {
  static char buf[80];
  switch (s->kind) {
    case T_MOVE: {
      T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
      if (dst->kind == T_MEM) {
        if (dst->u.MEM->kind == T_BINOP && dst->u.MEM->u.BINOP.op == T_plus &&
            dst->u.MEM->u.BINOP.right->kind == T_CONST) {
          /* MOVE(MEM(BINOP(PLUS,e1,CONST(i))),e2) */
          T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
          S("sw `s1, %d(`s0) # MOVE(MEM(BINOP(PLUS,e1,CONST(i))),e2)",
            dst->u.MEM->u.BINOP.right->u.CONST);
          emit(AS_Oper(STRDUP(buf), NULL,
                       L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
        } else if (dst->u.MEM->kind == T_BINOP &&
                   dst->u.MEM->u.BINOP.op == T_plus &&
                   dst->u.MEM->u.BINOP.left->kind == T_CONST) {
          /* MOVE(MEM(BINOP(PLUS, CONST(i),e1)),e2) */
          T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
          S("sw `s1, %d(`s0) # MOVE(MEM(BINOP(PLUS, CONST(i),e1)),e2)",
            dst->u.MEM->u.BINOP.left->u.CONST);
          emit(AS_Oper(STRDUP(buf), NULL,
                       L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
        } else {
          /* MOVE(MEM(e1),e2) */
          T_exp e1 = dst->u.MEM, e2 = src;
          emit(AS_Oper("sw `s1 0(`s0) # MOVE(MEM(e1),e2)", NULL,
                       L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
        }
      } else if (dst->kind == T_TEMP) {
        if (src->kind == T_CALL) {
          /* MOVE(TEMP(i),CALL(NAME(lab),args)) */
          assert(src->u.CALL.fun->kind == T_NAME);
          Temp_tempList l = munchArgs(0, src->u.CALL.args);
          // TODO
          S("call %s", Temp_labelstring(src->u.CALL.fun->u.NAME));
          emit(AS_Oper(STRDUP(buf), L(F_RV(), F_calldefs()), l, NULL));
          //
          emit(AS_Move("mv `d0, `s0 # copy return value", L(dst->u.TEMP, NULL),
                       L(F_RV(), NULL)));
        } else if (src->kind == T_CONST) {
          /* MOVE(TEMP(i),CONST(c)) */
          S("li `d0, %d # MOVE(TEMP(i),CONST(c)) T%d <- %d", src->u.CONST,
            dst->u.TEMP->num, src->u.CONST);
          emit(AS_Oper(STRDUP(buf), L(dst->u.TEMP, NULL), NULL, NULL));
        } else {
          /* MOVE(TEMP(i),e2) */
          T_exp e2 = src;
          Temp_temp i = dst->u.TEMP;
          Temp_temp j = munchExp(e2);
          S("mv `d0, `s0 # MOVE(TEMP(i),e2) T%d <- T%d", i->num, j->num);
          emit(AS_Move(STRDUP(buf), L(i, NULL), L(j, NULL)));
        }
      } else {
        assert(0);
      }
      break;
    }
    case T_JUMP: {
      if (s->u.JUMP.exp->kind == T_NAME) {
        /* JUMP(LABEL(t)) */
        emit(AS_Oper("j `j0 # JUMP(LABEL(t))", NULL, NULL,
                     AS_Targets(s->u.JUMP.jumps)));
      } else {
        /* JUMP(e) */
        emit(AS_Oper("j `s0 # JUMP(e)", NULL, L(munchExp(s->u.JUMP.exp), NULL),
                     AS_Targets(s->u.JUMP.jumps)));
      }
      break;
    }
    case T_CJUMP: {
      // note that in chap8, we have manipulated all the CJUMP blocks so that
      // the instructions follows are false branch
      if (s->u.CJUMP.left->kind == T_CONST &&
          s->u.CJUMP.right->kind == T_CONST) {
        debug("CJUMP: is comparing two constants\n");
        // TODO constexpr
      }

      // TODO: pseudo instruction for compare to 0
      Temp_temp e1 = munchExp(s->u.CJUMP.left);
      Temp_temp e2 = munchExp(s->u.CJUMP.right);
      // Temp_temp r1 = Temp_newtemp();
      // Temp_temp r2 = Temp_newtemp();
      // // FIXME: Do we need this safe copy?
      // emit(AS_Move("mv `d0, `s0 # safe copy of e1", L(r1, NULL), L(e1,
      // NULL))); emit(AS_Move("mv `d0, `s0 # safe copy of e2", L(r2, NULL),
      // L(e2, NULL)));

      char *op_code;
      switch (s->u.CJUMP.op) {
        case T_eq: op_code = "beq"; break;
        case T_ne: op_code = "bne"; break;
        case T_lt: op_code = "blt"; break;
        case T_gt: op_code = "bgt"; break;
        case T_le: op_code = "ble"; break;
        case T_ge: op_code = "bge"; break;
        default: assert(0);
      }
      S("%s `s0, `s1, `j0 # compare e1 e2, true '%s', false '%s'", op_code,
        Temp_labelstring(s->u.CJUMP.true), Temp_labelstring(s->u.CJUMP.false));
      emit(AS_Oper(
          STRDUP(buf), NULL, L(e1, L(e2, NULL)),
          AS_Targets(Temp_LabelList(s->u.CJUMP.true,
                                    Temp_LabelList(s->u.CJUMP.false, NULL)))));
      S("j `j0 # jump to false");
      emit(AS_Oper(STRDUP(buf), NULL, NULL,
                   AS_Targets(Temp_LabelList(s->u.CJUMP.false, NULL))));
      break;
    }
    case T_LABEL: {
      S("%s:", Temp_labelstring(s->u.LABEL));
      emit(AS_Label(STRDUP(buf), s->u.LABEL));
      break;
    }
    case T_EXP: {
      munchExp(s->u.EXP);
      break;
    }
    default: assert(0);
  }
}

static Temp_temp munchExp(T_exp e) {
  static char buf[80];
  Temp_temp r = Temp_newtemp();
  switch (e->kind) {
    case T_BINOP: {
      // note that we have converted all "and" and "or" operators to if..else
      T_exp e1 = e->u.BINOP.left;
      T_exp e2 = e->u.BINOP.right;
      T_binOp op = e->u.BINOP.op;
      if (e1->kind == T_CONST && e2->kind == T_CONST) {
        /* BINOP(*,CONST(i1),CONST(i2)) */
        int a = e1->u.CONST, b = e2->u.CONST, result;
        switch (op) {
          case T_plus: result = a + b; break;
          case T_minus: result = a - b; break;
          case T_mul: result = a * b; break;
          case T_div: result = a / b; break;
          default: assert(0);
        }
        S("li `d0, %d # result of constexpr", result);
        emit(AS_Oper(STRDUP(buf), L(r, NULL), NULL, NULL));
        return r;
      } else if ((e1->kind == T_CONST || e2->kind == T_CONST) &&
                 (op == T_plus || op == T_minus)) {
        /* BINOP(PLUS,CONST(i),e2) */
        /* BINOP(PLUS,e1,CONST(i)) */
        /* BINOP(MINUS,CONST(i),e2) */
        /* BINOP(MINUS,e1,CONST(i)) */
        T_exp var = e1->kind == T_CONST ? e2 : e1;
        int constt = e1->kind == T_CONST ? e1->u.CONST : e2->u.CONST;
        switch (op) {
          case T_plus: S("addi `d0, `s0, %d", constt); break;
          case T_minus: S("addi `d0, `s0, -%d", constt); break;
          default: assert(0);
        }
        emit(AS_Oper(STRDUP(buf), L(r, NULL), L(munchExp(var), NULL), NULL));
        return r;
      } else {
        /* BINOP(*,e1,e2) */
        Temp_temp e1r = munchExp(e1);
        Temp_temp e2r = munchExp(e2);
        switch (op) {
          case T_plus:
            S("add `d0, `s0, `s1 # T%d <- T%d + T%d", r->num, e1r->num,
              e2r->num);
            break;
          case T_minus:
            S("sub `d0, `s0, `s1 # T%d <- T%d - T%d", r->num, e1r->num,
              e2r->num);
            break;
          case T_mul:
            S("mul `d0, `s0, `s1 # T%d <- T%d * T%d", r->num, e1r->num,
              e2r->num);
            break;
          case T_div:
            S("div `d0, `s0, `s1 # T%d <- T%d / T%d", r->num, e1r->num,
              e2r->num);
            break;
          default: assert(0);
        }
        emit(AS_Oper(STRDUP(buf), L(r, NULL), L(e1r, L(e2r, NULL)), NULL));
        return r;
      }
    }
    case T_MEM: {
      if (e->u.MEM->kind == T_BINOP) {
        T_exp e1 = e->u.MEM->u.BINOP.left;
        T_exp e2 = e->u.MEM->u.BINOP.right;
        T_binOp op = e->u.MEM->u.BINOP.op;
        if (op == T_plus && e2->kind == T_CONST) {
          /* MEM(BINOP(PLUS,e1,CONST(i))) */
          S("lw `d0, %d(`s0) # MEM(BINOP(PLUS,e1,CONST(i)))", e2->u.CONST);
          emit(AS_Oper(STRDUP(buf), L(r, NULL), L(munchExp(e1), NULL), NULL));
          return r;
        } else if (op == T_plus && e1->kind == T_CONST) {
          /* MEM(BINOP(PLUS,CONST(i),e2)) */
          S("lw `d0, %d(`s0) # MEM(BINOP(PLUS,CONST(i),e2))", e1->u.CONST);
          emit(AS_Oper(STRDUP(buf), L(r, NULL), L(munchExp(e2), NULL), NULL));
          return r;
        } else {
          /* MEM(e) */
          emit(AS_Oper("mv `d0, `s0 # MEM(e)", L(r, NULL),
                       L(munchExp(e->u.MEM), NULL), NULL));
          return r;
        }
      } else if (e->u.MEM->kind == T_CONST) {
        /* MEM(CONST(i)) */
        S("li `d0, %d # MEM(CONST(i))", e->u.MEM->u.CONST);
        emit(AS_Oper(STRDUP(buf), L(r, NULL), NULL, NULL));
        return r;
      } else {
        /* MEM(e) */
        emit(AS_Oper("mv `d0, `s0 # MEM(e)", L(r, NULL),
                     L(munchExp(e->u.MEM), NULL), NULL));
        return r;
      }
    }
    case T_TEMP: {
      /* TEMP(t) */
      return e->u.TEMP;
    }
    case T_CONST: {
      /* CONST(i) */
      S("li `d0, %d # load CONST(i) to T%d", e->u.CONST, r->num);
      emit(AS_Oper(STRDUP(buf), L(r, NULL), NULL, NULL));
      return r;
    }
    case T_NAME: {
      /* NAME(lab) */
      S("la `d0, %s # NAME(lab)", Temp_labelstring(e->u.NAME));
      emit(AS_Oper(STRDUP(buf), L(r, NULL), NULL, NULL));
      return r;
    }
    case T_CALL: {
      /* CALL(NAME(lab), args) */
      assert(e->u.CALL.fun->kind == T_NAME);
      Temp_tempList l = munchArgs(0, e->u.CALL.args);
      // TODO
      S("call %s", Temp_labelstring(e->u.CALL.fun->u.NAME));
      emit(AS_Oper(STRDUP(buf), F_calldefs(), l, NULL));
      //
      return r;
    }
    default: assert(0);
  }
}

/**
 * @brief
 * @param stmt2last_instr  Set<stmt_instr<stmt, AS_instr>>
 * @return
 */
AS_instrList F_codegen(Set stmt2last_instr, F_frame f, T_stmList stmList) {
  T_stmList sl;
  AS_instrList list = NULL;

  // FIXME
  // single jump instruction was ignored

  struct stmt_instr *new = checked_malloc(sizeof(*new));
  new->stm = NULL;
  for (sl = stmList; sl; sl = sl->tail) {
    munchStm(sl->head);
    if (new->stm && last->head->kind == I_LABEL &&new->last->kind != I_LABEL) {
      SET_insert(stmt2last_instr, new);
      new = checked_malloc(sizeof(*new));
      new->stm = NULL;
    }
    new->stm = sl->head;
    new->last = last->head;
  }
  list = iList;
  iList = last = NULL;
  return list;
}

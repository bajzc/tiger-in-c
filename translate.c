/* translate.c */
#include "translate.h"
#include "absyn.h"
#include "errormsg.h"
#include "frame.h"
#include "printtree.h"
#include "temp.h"
#include "tree.h"
#include "util.h"

static Tr_level OUTER_MOST = NULL;
static F_fragList F_fragments = NULL;

static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);
static patchList PatchList(Temp_label *head, patchList tail);
static void doPatch(patchList tList, Temp_label label);
static patchList joinPatch(patchList first, patchList second);
static T_exp unEx(Tr_exp e);
static struct Cx unCx(Tr_exp e);

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level) {
  T_exp f = F_Exp(access->access, T_Temp(F_FP()));
  if (access->level != level) {
    // TODO different `level` mean it's in different frame?
    debug("%p : %p access via static links\n", access->level, level);
    Tr_level l = level->parent;
    T_exp e = T_Mem(T_Temp(F_FP()));
    while (l != access->level) {
      e = T_Mem(e);
      l = l->parent;
    }
    f->u.MEM->u.BINOP.left = e; // TODO to be confirmed
  }
  return Tr_Ex(f);
}

// record.f -> record.offset(f)
Tr_exp Tr_fieldVar(Tr_exp record, int offset) {
  // TODO add NULL checker
  if (record->kind != Tr_ex)
    assert(0);
  return Tr_Ex(
      T_Mem(T_Binop(T_plus, unEx(record), T_Const(F_wordSize * offset))));
}

// a[i] -> MEM(+(MEM(a)), BINOP(MUL, i, CONST WordSize))
Tr_exp Tr_subscriptVar(Tr_exp arr, Tr_exp index) {
  // TODO add index range checker and NULL checker
  if (arr->kind != Tr_ex || index->kind != Tr_ex)
    assert(0);
  return Tr_Ex(T_Mem(T_Binop(
      T_plus, unEx(arr), T_Binop(T_mul, unEx(index), T_Const(F_wordSize)))));
}

Tr_exp Tr_nilExp(void) { return Tr_Ex(T_Const(0)); }

Tr_exp Tr_intExp(int a) { return Tr_Ex(T_Const(a)); }

Tr_exp Tr_stringExp(string s) {
  Temp_label lab = Temp_newlabel();
  F_frag f = F_StringFrag(lab, s);
  F_fragments = F_FragList(f, F_fragments);
  debug("str: '%s' as label '%s'\n", s, Temp_labelstring(lab));
  return Tr_Ex(T_Name(lab));
}

Tr_exp Tr_callExp(Temp_label name, Tr_exp *argv, int argc,
                  Tr_level caller_level, Tr_level callee_level) {
  T_expList l = NULL, l_head = NULL;
  for (int i = 0; i < argc; i++) {
    l = T_ExpList(unEx(argv[i]), l);
    if (i == 0)
      l_head = l;
    l = l->tail;
  }
  Tr_level c = caller_level;
  T_exp static_link = T_Temp(F_FP());

  if (callee_level->parent != c) {
    while (c) {
      static_link = T_Mem(static_link);
      if (callee_level->parent == c->parent)
        break;
      c = c->parent;
    }
  }
  return Tr_Ex(T_Call(T_Name(name), T_ExpList(static_link, l_head)));
}

// string comparison is not handled here
Tr_exp Tr_opExp(Tr_exp l, A_oper op, Tr_exp r) {
  T_exp L = unEx(l), R = unEx(r);
  T_stm s = NULL;
  switch (op) {
    case A_plusOp: return Tr_Ex((T_Binop(T_plus, L, R)));
    case A_minusOp: return Tr_Ex((T_Binop(T_minus, L, R)));
    case A_timesOp: return Tr_Ex((T_Binop(T_mul, L, R)));
    case A_divideOp: return Tr_Ex((T_Binop(T_div, L, R)));
    case A_eqOp: s = T_Cjump(T_eq, L, R, NULL, NULL); goto patch;
    case A_neqOp: s = T_Cjump(T_ne, L, R, NULL, NULL); goto patch;
    case A_ltOp: s = T_Cjump(T_lt, L, R, NULL, NULL); goto patch;
    case A_leOp: s = T_Cjump(T_le, L, R, NULL, NULL); goto patch;
    case A_gtOp: s = T_Cjump(T_gt, L, R, NULL, NULL); goto patch;
    case A_geOp:
      s = T_Cjump(T_ge, L, R, NULL, NULL);
    patch:
      return Tr_Cx(PatchList(&(s->u.CJUMP.true), NULL),
                   PatchList(&(s->u.CJUMP.false), NULL), s);
  }
  assert(0);
}

Tr_exp Tr_eqExpString(Tr_exp l, A_oper op, Tr_exp r) {
  // int stringEqual(char* l, char* r);
  assert(op == A_eqOp || op == A_neqOp);
  return Tr_Ex(F_externalCall(
      "stringEqual",
      T_ExpList(unEx(l), T_ExpList(unEx(r), NULL))));
}

/**
 * @brief use runtime function to init record on heap
 * @param l list of field expr
 * @param size number of fields in record
 */
Tr_exp Tr_recordExp(Tr_exp *l, int size) {
  assert(size > 0);
  T_exp r = T_Temp(Temp_newtemp());
  // initRecord(size_t size_in_byte);
  T_exp res_head = T_Eseq(
      T_Seq(T_Move(r,
                   F_externalCall("initRecord",
                                  T_ExpList(T_Const(size * F_wordSize), NULL))),
            NULL),
      r);
  T_stm *rightmost = &(res_head->u.ESEQ.stm);
  for (int i = 0; i < size; i++) {
    if (l[i] != NULL) {
      rightmost = &((*rightmost)->u.SEQ.right);
      *rightmost =
          T_Seq(T_Move(T_Mem(T_Binop(T_plus, r, T_Const(i * F_wordSize))),
                       l[i]->u.ex),
                NULL);
    }
  }
  *rightmost = ((*rightmost)->u.SEQ.left);
  return Tr_Ex(res_head);
}

Tr_exp Tr_arrayExp(Tr_exp init, int size) {
  T_exp a = T_Temp(Temp_newtemp());
  return Tr_Ex(T_Eseq(
      T_Move(a, F_externalCall(
                    "initArray",
                    T_ExpList(unEx(init), T_ExpList(T_Const(size), NULL)))),
      a));
}

Tr_exp Tr_seqExp(Tr_exp *seqs, int size) {
  if (size < 1) {
    EM_error((A_pos) {0}, "bug? this should cause a syntax error");
  } else if (size == 1) {
    return seqs[0];
  } else if (size == 2) {
    return Tr_Ex(T_Eseq(unNx(seqs[0]), unEx(seqs[1])));
  }
  // no generate NULL at tail
  T_exp seq = T_Eseq(NULL, unEx(seqs[size - 1]));
  T_stm *rightmost = &seq->u.ESEQ.stm;
  for (int i = 0; i < size - 2; i++) {
    *rightmost = T_Seq(unNx(seqs[i]), NULL);
    rightmost = &(*rightmost)->u.SEQ.right;
  }
  *rightmost = unNx(seqs[size - 2]);
  return Tr_Ex(seq);
}

Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp exp) {
  return Tr_Nx(T_Move(unEx(lvalue), unEx(exp)));
}

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee) {
  // TODO optimise
  // if (elsee != NULL) {
  //   debug("if-else-then statement\n");
  //   if (then->kind == Tr_nx && elsee->kind == Tr_nx) {
  //     debug("'then' and 'elsee' are statements\n");
  //     T_stm st = NULL, sf = NULL;
  //     Temp_label t = Temp_newlabel(), f = Temp_newlabel();
  //     st = T_Seq(T_Label(t), then->u.nx);
  //     sf = T_Seq(T_Label(f), elsee->u.nx);
  //     doPatch(test->u.cx.trues, t);
  //     doPatch(test->u.cx.falses, f);
  //     return Tr_Nx((T_Seq(unCx(test).stm, T_Seq(st, sf))));
  //   } else if (then->kind == Tr_cx || elsee->kind == Tr_cx) {
  //     debug("'then' or 'elsee' is conditional statement\n");
  //     T_stm st = NULL, sf = NULL;
  //     Temp_label t= Temp_newlabel(), f=Temp_newlabel();
  //     if(then->kind==Tr_cx){
  //
  //     }
  //  } else {
  //     debug("tradt 'then' and 'elsee' as expressions\n");
  //   }
  // } else {
  //   debug("if-else statement\n");
  //   if (then->kind == Tr_nx) {
  //     debug("'then' is a statement\n");
  //  } else if (then->kind == Tr_cx) {
  //     debug("'then' is a conditional statement\n");
  //   } else {
  //     debug("treat 'then' as expression\n");
  //   }
  // }
  struct Cx cx = unCx(test);
  Temp_temp r = Temp_newtemp(); // result
  Temp_label t = Temp_newlabel(), f = Temp_newlabel();
  Temp_label merge = Temp_newlabel();
  doPatch(cx.trues, t);
  doPatch(cx.falses, f);
  if (elsee != NULL) {
    T_exp then_t = unEx(then);
    T_exp elsee_t = unEx(elsee);
    return Tr_Ex(T_Eseq(
        T_Seq(cx.stm,
              T_Seq(T_Seq(T_Seq(T_Label(t),
                                T_Seq(T_Move(T_Temp(r), then_t),
                                      T_Jump(T_Name(merge),
                                             Temp_LabelList(merge, NULL)))),
                          T_Seq(T_Label(f),
                                T_Seq(T_Move(T_Temp(r), elsee_t),
                                      T_Jump(T_Name(merge),
                                             Temp_LabelList(merge, NULL))))),
                    T_Label(merge))),
        T_Temp(r)));
  } else {
    T_exp then_t = unEx(then);
    return Tr_Nx(T_Seq(
        cx.stm,
        T_Seq(
            T_Seq(T_Seq(T_Label(t), T_Seq(T_Move(T_Temp(r), then_t),
                                          T_Jump(T_Name(merge),
                                                 Temp_LabelList(merge, NULL)))),
                  T_Seq(T_Label(f),
                        T_Jump(T_Name(merge), Temp_LabelList(merge, NULL)))),
            T_Label(merge))));
  }
}

Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp body, Temp_label done) {
  struct Cx cx = unCx(cond);
  Temp_label t = Temp_newlabel();
  Temp_label test = Temp_newlabel();
  doPatch(cx.trues, t);
  doPatch(cx.falses, done);
  return Tr_Nx(T_Seq(
      T_Label(test),
      T_Seq(cx.stm,
            T_Seq(T_Label(t),
                  T_Seq(unNx(body),
                        T_Seq(T_Jump(T_Name(test), Temp_LabelList(test, NULL)),
                              T_Label(done)))))));
}

Tr_exp Tr_forExp(Tr_exp body, Tr_exp var, Tr_exp lo, Tr_exp hi,
                 Temp_label done) {
  Temp_label test = Temp_newlabel(), body_start = Temp_newlabel();
  return Tr_Nx(T_Seq(
      T_Move(unEx(var), unEx(lo)),
      T_Seq(T_Label(test),
            T_Seq(T_Cjump(T_le, unEx(var), unEx(hi), body_start, done),
                  T_Seq(T_Label(body_start),
                        T_Seq(unNx(body),
                              T_Seq(T_Move(unEx(var), T_Binop(T_plus, unEx(var),
                                                              T_Const(1))),
                                    T_Seq(T_Jump(T_Name(test),
                                                 Temp_LabelList(test, NULL)),
                                          T_Label(done)))))))));
}

Tr_exp Tr_breakExp(Temp_label done) {
  return Tr_Nx(T_Jump(T_Name(done), Temp_LabelList(done, NULL)));
}

Tr_exp Tr_letExp(Tr_exp *decs, int size, Tr_exp body) {
  T_exp exp = unEx(body);
  for (int i = 0; i < size; i++) {
    if (decs[i] != NULL)
      exp = T_Eseq(unNx(decs[i]), exp);
  }
  return Tr_Ex(exp);
}

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals,
                      bool return_flag) {
  T_stm stm = unNx(body);
  if (return_flag)
     stm = T_Move(T_Temp(F_RV()), unEx(body));
  F_frag frag = F_ProcFrag(stm, level->frame);
  F_fragments = F_FragList(frag, F_fragments);
  F_procEntryExit1(level->frame, stm);
  debug("call printStmList on function '%s'\n", Temp_labelstring(level->name));
  printStmList(stderr, T_StmList(stm, NULL));
}

static Tr_exp Tr_Ex(T_exp ex) {
  Tr_exp p = checked_malloc(sizeof(*p));
  p->kind = Tr_ex;
  p->u.ex = ex;
  return p;
}

static Tr_exp Tr_Nx(T_stm nx) {
  Tr_exp p = checked_malloc(sizeof(*p));
  p->kind = Tr_nx;
  p->u.nx = nx;
  return p;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm) {
  Tr_exp p = checked_malloc(sizeof(*p));
  p->kind = Tr_cx;
  p->u.cx.stm = stm;
  p->u.cx.trues = trues;
  p->u.cx.falses = falses;
  return p;
}

static patchList PatchList(Temp_label *head, patchList tail) {
  patchList p = checked_malloc(sizeof(*p));
  p->head = head;
  p->tail = tail;
  return p;
}

void doPatch(patchList tList, Temp_label label) {
  for (; tList; tList = tList->tail)
    *(tList->head) = label;
}

patchList joinPatch(patchList first, patchList second) {
  if (!first)
    return second;
  for (; first->tail; first = first->tail) // go to end of list
    ;
  first->tail = second;
  return first;
}

static T_exp unEx(Tr_exp e) {
  assert(e);
  switch (e->kind) {
    case Tr_ex: return e->u.ex;
    case Tr_cx: {
      Temp_temp r = Temp_newtemp();
      Temp_label t = Temp_newlabel(), f = Temp_newlabel();
      doPatch(e->u.cx.trues, t);
      doPatch(e->u.cx.falses, f);
      return T_Eseq(
          T_Move(T_Temp(r), T_Const(1)),
          T_Eseq(e->u.cx.stm,
                 T_Eseq(T_Label(f), T_Eseq(T_Move(T_Temp(r), T_Const(0)),
                                           T_Eseq(T_Label(t), T_Temp(r))))));
    }
    case Tr_nx: return T_Eseq(e->u.nx, T_Const(0));
  }
  assert(0);
}

T_stm unNx(Tr_exp e) {
  debug2("call unNx\n");
  assert(e);
  switch (e->kind) {
    case Tr_ex: return T_Exp(e->u.ex);
    case Tr_cx: {
      Temp_temp r = Temp_newtemp();
      Temp_label t = Temp_newlabel(), f = Temp_newlabel();
      doPatch(e->u.cx.trues, t);
      doPatch(e->u.cx.falses, f);
      return T_Seq(
          T_Move(T_Temp(r), T_Const(1)),
          T_Seq(e->u.cx.stm,
                T_Seq(T_Label(f), T_Seq(T_Move(T_Temp(r), T_Const(0)),
                                        T_Seq(T_Label(t), T_Exp(T_Temp(r)))))));
    }
    case Tr_nx: return e->u.nx;
  }
  assert(0);
}

static struct Cx unCx(Tr_exp e) {
  debug2("call unCx()\n");
  switch (e->kind) {
    case Tr_ex: {
      if (e->u.ex->kind == T_CONST) {
        Temp_label t = Temp_newlabel();
        debug("optimise for if(%d)\n", e->u.ex->u.CONST);
        T_stm s = T_Jump(T_Name(t), Temp_LabelList(t, NULL));
        struct Cx cx = (struct Cx) {NULL, NULL, s};
        if (e->u.ex->u.CONST == 0) {
          cx.falses = PatchList(&cx.stm->u.JUMP.jumps->head, NULL);
        } else {
          cx.trues = PatchList(&cx.stm->u.JUMP.jumps->head, NULL);
        }
        return cx;
      }
      T_stm s = T_Cjump(T_ne, unEx(e), T_Const(0), NULL, NULL);
      patchList t = PatchList(&s->u.CJUMP.true, NULL);
      patchList f = PatchList(&s->u.CJUMP.false, NULL);
      return Tr_Cx(t, f, s)->u.cx;
    }
    case Tr_cx: return e->u.cx;
    case Tr_nx: assert(0);
  }
  assert(0);
}

Tr_level Tr_outermost(void) {
  if (!OUTER_MOST) {
    debug("init outermost level\n");
    Tr_level t = checked_malloc(sizeof(*t));
    t->parent = NULL;
    t->name = Temp_namedlabel("prelude");
    t->formals = NULL;
    t->frame = F_newFrame(Temp_namedlabel("global"), NULL);
    OUTER_MOST = t;
    return t;
  }
  return OUTER_MOST;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
  Tr_access t = checked_malloc(sizeof(*t));
#if DEBUG
  if (level == OUTER_MOST)
    debug("alloc to global scape\n");
#endif
  t->level = level;
  t->access = F_allocLocal(level->frame, escape);
  return t;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
  Tr_level level = checked_malloc(sizeof(*level));
  level->parent = parent;
  level->name = name;
  level->formals = formals;
  // add the static links to the first place, rest is in order
  level->frame = F_newFrame(name, U_BoolList(TRUE, formals));
  return level;
}

Tr_accessList Tr_formals(Tr_level level) {
  F_accessList f = F_formals(level->frame);
  if (!f && level == OUTER_MOST)
    return NULL;
  else
    assert(f);
  Tr_accessList l = NULL, l_head = NULL;
  // builtin functions will not occur here
  f = f->tail; // skip static link
  if (f == NULL) {
    return NULL;
  }
  l = Tr_AccessList(Tr_Access(level, f->head), NULL);
  l_head = l;
  f = f->tail;
  for (; f; f = f->tail, l = l->tail) {
    Tr_accessList p = Tr_AccessList(Tr_Access(level, f->head), NULL);
    l->tail = p;
  }
  return l_head;
}

void Tr_printFormals(Tr_accessList formals) {
  debug("vvvvvvvvvv\n");
  Tr_access p = NULL;
  while (formals) {
    p = formals->head;
    F_printAccess(p->access);
    formals = formals->tail;
  }
  fflush(stdout);
  debug("^^^^^^^^^^\n");
}

F_fragList Tr_getResult(void) { return F_fragments; }

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
  Tr_accessList t = checked_malloc(sizeof(*t));
  t->head = head;
  t->tail = tail;
  return t;
}

Tr_access Tr_Access(Tr_level level, F_access access) {
  Tr_access a = checked_malloc(sizeof(*a));
  a->level = level;
  a->access = access;
  return a;
}

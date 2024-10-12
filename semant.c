/* semant.c */
#include "semant.h"
#include "absyn.h"
#include "env.h"
#include "errormsg.h"
#include "escape.h"
#include "symbol.h"
#include "translate.h"
#include "types.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

static int breakLevel = 0;
// type check
// used in transExp()
#define OPEXP_INT_CHECK(l, r, a)                                               \
  do {                                                                         \
    if (l != Ty_int)                                                           \
      EM_error(a->u.op.left->pos, "integer required");                         \
    if (r != Ty_int)                                                           \
      EM_error(a->u.op.right->pos, "integer required");                        \
  } while (0);

typedef void *Tr_exp;
struct expty {
  Tr_exp exp;
  Ty_ty ty;
};

static char str_ty[][12] = {"ty_record", "ty_nil",  "ty_int", "ty_string",
                            "ty_array",  "ty_name", "ty_void"};

struct expty expTy(Tr_exp exp, Ty_ty ty) {
  struct expty e;
  e.exp = exp;
  e.ty = ty;
  return e;
}

static void beginBreakScope() { breakLevel++; }

static void endBreakScope() { breakLevel--; }

static Ty_ty actual_ty(Ty_ty ty) {
  if (ty == NULL)
    EM_error((A_pos){0}, "definition of recursive types is not completed");
  Ty_ty ty_start; // avoid infinite loop
  ty_start = ty;
  while (ty->kind == Ty_name) {
    ty = ty->u.name.ty;
    if (ty == ty_start)
      EM_error((A_pos){0}, "infinite loop of recursive type");
  }
  return ty;
}

static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params) {
  Ty_tyList l, l_head = NULL;
  if (params) {
    l = Ty_TyList(actual_ty(S_look(tenv, params->head->typ)), NULL);
    l_head = l;
    params = params->tail;
  }
  for (; params; params = params->tail, l = l->tail) {
    l->tail = Ty_TyList(actual_ty(S_look(tenv, params->head->typ)), NULL);
  }
  /* TyList_print(l_head); */
  /* debug("\n"); */
  return l_head;
}

static Ty_ty findFieldInRecord(Ty_fieldList record, S_symbol sym) {
  assert(record);
  assert(sym);
  Ty_fieldList h;
  for (h = record; h; h = h->tail) {
    if (h->head->name == sym)
      return actual_ty(h->head->ty);
  }
  return NULL;
}

struct expty transExp(S_table venv, S_table tenv, A_exp a, Tr_level level) {
  if (a == NULL)
    return expTy(NULL, Ty_Void());
  switch (a->kind) {
  case A_varExp: {
    return transVar(venv, tenv, a->u.var, level);
  }
  case A_nilExp:
    return expTy(NULL, Ty_Nil());
  case A_intExp:
    return expTy(NULL, Ty_Int());
  case A_stringExp:
    return expTy(NULL, Ty_String());
  case A_callExp: {
    E_enventry funTy = S_look(venv, a->u.call.func);
    if (funTy && funTy->kind == E_funEntry) {
      Ty_tyList l = funTy->u.fun.formals;
      A_expList e = a->u.call.args;
      while (e && l) {
        struct expty eTy = transExp(venv, tenv, e->head, level);
        if (eTy.ty->kind == Ty_nil && l->head->kind == Ty_record)
          ; // OK
        else if (eTy.ty->kind != l->head->kind)
          EM_error(e->head->pos,
                   "expected '%s' but argument is of "
                   "type '%s'",
                   str_ty[l->head->kind], str_ty[eTy.ty->kind]);
        if (actual_ty(eTy.ty) != actual_ty(l->head)) {
          if (eTy.ty->kind == Ty_array)
            EM_error(e->head->pos,
                     "different array type as function "
                     "was defined %p:%p",
                     actual_ty(eTy.ty), actual_ty(l->head));
          else if (eTy.ty->kind == Ty_record)
            EM_error(e->head->pos,
                     "different array type as function "
                     "was defined %p:%p",
                     actual_ty(eTy.ty), actual_ty(l->head));
          else
            EM_error(e->head->pos, "UNKNOWN type different");
        }
        e = e->tail;
        l = l->tail;
      }
      if (e)
        EM_error(a->pos, "passed more arguments than needed");
      if (l)
        EM_error(a->pos, "passed less arguments than needed");
      return expTy(NULL, actual_ty(funTy->u.fun.result));
    } else
      EM_error(a->pos, "function '%s' not declared", S_name(a->u.call.func));
  }
  case A_opExp: {
    A_oper oper = a->u.op.oper;
    struct expty left = transExp(venv, tenv, a->u.op.left, level);
    struct expty right = transExp(venv, tenv, a->u.op.right, level);
    int l = left.ty->kind;
    int r = right.ty->kind;
    switch (oper) {
    case A_plusOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_minusOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_timesOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_divideOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_eqOp:
      if ((l == Ty_record && r == Ty_nil) || (r == Ty_nil && l == Ty_record))
        return expTy(NULL, Ty_Int());
      if (l != r)
        EM_error(a->pos, "same type required for both sides");
      else if (l != Ty_int && l != Ty_record && l != Ty_array && l != Ty_string)
        EM_error(a->pos,
                 "only integer, record or array can be compared (%s = %s)",
                 str_ty[l], str_ty[r]);
      return expTy(NULL, Ty_Int());
    case A_neqOp:
      if ((l == Ty_record && r == Ty_nil) || (r == Ty_nil && l == Ty_record))
        return expTy(NULL, Ty_Int());
      if (l != r)
        EM_error(a->pos, "same type required for both sides");
      if (l != Ty_int && l != Ty_record && l != Ty_array && l != Ty_string)
        EM_error(a->pos,
                 "only integer, record or array can be compared (%s <> %s)",
                 str_ty[l], str_ty[r]);
      return expTy(NULL, Ty_Int());
    case A_ltOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_leOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_gtOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    case A_geOp:
      OPEXP_INT_CHECK(l, r, a);
      return expTy(NULL, Ty_Int());
    default:
      assert(0);
    } // end switch(oper)
  } // end case A_opExp
  case A_recordExp: {
    Ty_ty ty = actual_ty(S_look(tenv, a->u.record.typ));
    if (ty == NULL)
      EM_error(a->pos, "type '%s' is not defined", S_name(a->u.record.typ));
    if (ty->kind != Ty_record)
      EM_error(a->pos, "'%s' is not a record type", S_name(a->u.record.typ));
    for (A_efieldList l = a->u.record.fields; l; l = l->tail) {
      Ty_ty fieldTy = findFieldInRecord(ty->u.record, l->head->name);
      if (fieldTy == NULL)
        EM_error(a->pos, "no member named '%s' in the record",
                 S_name(l->head->name));
      struct expty eTy = transExp(venv, tenv, l->head->exp, level);
      if (fieldTy->kind == Ty_record && eTy.ty->kind == Ty_nil)
        ; // OK
      else if (eTy.ty != fieldTy)
        EM_error(a->pos,
                 "incompatible tpye(%s:%p->%s:%p) when "
                 "initial '%s'",
                 str_ty[eTy.ty->kind], eTy.ty, str_ty[fieldTy->kind], fieldTy);
    }
    return expTy(NULL, actual_ty(S_look(tenv, a->u.record.typ)));
  }
  case A_seqExp: {
    A_expList l;
    struct expty last_ty;
    if (a->u.seq == NULL)
      return expTy(NULL, Ty_Void());
    for (l = a->u.seq; l; l = l->tail) {
      last_ty = transExp(venv, tenv, l->head, level);
    }
    return last_ty;
  }
  case A_assignExp: {
    struct expty lvalue = transVar(venv, tenv, a->u.assign.var, level);
    struct expty exp = transExp(venv, tenv, a->u.assign.exp, level);
    if (lvalue.ty->kind == Ty_record && exp.ty->kind == Ty_nil)
      ; // OK
    else if (lvalue.ty->kind != exp.ty->kind)
      EM_error(a->pos, "assign %s type to %s\n", str_ty[exp.ty->kind],
               str_ty[lvalue.ty->kind]);
    return expTy(NULL, Ty_Void());
  }
  case A_ifExp: {
    struct expty test, then, elsee;
    assert(a->u.iff.test);
    assert(a->u.iff.then);
    test = transExp(venv, tenv, a->u.iff.test, level);
    if (test.ty->kind != Ty_int)
      EM_error(a->u.iff.test->pos,
               "result of condition should be an integer value (%s)",
               str_ty[test.ty->kind]);
    then = transExp(venv, tenv, a->u.iff.then, level);
    if (a->u.iff.elsee) { // if-then-else
      elsee = transExp(venv, tenv, a->u.iff.elsee, level);
      if (then.ty->kind != elsee.ty->kind) {
        // see LANG MANUAL A.3.2.nil and MERGE.TIG:22-28
        if (then.ty->kind == Ty_nil && elsee.ty->kind == Ty_record)
          return expTy(NULL, actual_ty(elsee.ty));
        else if (then.ty->kind == Ty_record && elsee.ty->kind == Ty_nil)
          return expTy(NULL, actual_ty(then.ty));
        else
          EM_error(
              a->pos,
              "ifExp: expression of then(%s) and else(%s) does not have the "
              "same type",
              str_ty[then.ty->kind], str_ty[elsee.ty->kind]);
      }
      return expTy(NULL, actual_ty(elsee.ty));
    } else { // if-then
      if (then.ty->kind != Ty_void)
        EM_error(a->u.iff.then->pos,
                 "expression of then-body(%s) must produce no value",
                 str_ty[then.ty->kind]);
      return expTy(NULL, Ty_Void());
    }
    assert(0);
  }
  case A_whileExp: {
    struct expty test, body;
    assert(a->u.whilee.test);
    test = transExp(venv, tenv, a->u.whilee.test, level);
    if (test.ty->kind != Ty_int)
      EM_error(a->u.whilee.test->pos,
               "result of condition should be an integer value (%s)",
               str_ty[test.ty->kind]);
    beginBreakScope();
    body = transExp(venv, tenv, a->u.whilee.body, level);
    endBreakScope();
    if (body.ty->kind != Ty_void)
      EM_error(a->u.whilee.body->pos,
               "expression of while-body(%s) must produce no value",
               str_ty[body.ty->kind]);
    return expTy(NULL, Ty_Void());
  }
  case A_forExp: {
    struct expty lo, hi;
    assert(a->u.forr.lo);
    assert(a->u.forr.hi);
    assert(a->u.forr.body);
    lo = transExp(venv, tenv, a->u.forr.lo, level);
    hi = transExp(venv, tenv, a->u.forr.hi, level);
    if (lo.ty->kind != Ty_int)
      EM_error(a->u.forr.lo->pos, "initial value(%s) should be an integer",
               str_ty[lo.ty->kind]);
    if (hi.ty->kind != Ty_int)
      EM_error(a->u.forr.hi->pos, "upper bound(%s) should be an integer",
               str_ty[hi.ty->kind]);
    S_beginScope(venv, 1);
    S_beginScope(tenv, 0);
    beginBreakScope();
    {
      // TODO should not assign to var in this scope
      S_enter(venv, a->u.forr.var,
              E_VarEntry(lo.ty, Tr_allocLocal(level, a->u.forr.escape)));
      struct expty body = transExp(venv, tenv, a->u.forr.body, level);
      if (body.ty->kind != Ty_void)
        EM_error(a->u.forr.body->pos,
                 "for-loop's body must produce no value (%s)",
                 str_ty[body.ty->kind]);
    }
    S_endScope(tenv, 0);
    S_endScope(venv, 1);
    endBreakScope();
    return expTy(NULL, Ty_Void());
  }
  case A_breakExp:
    if (breakLevel <= 0)
      EM_error(a->pos, "'break' statement not in loop");
    return expTy(NULL, Ty_Void());
  case A_letExp: {
    struct expty exp;
    A_decList d;
    S_beginScope(venv, 1);
    S_beginScope(tenv, 0);
    for (d = a->u.let.decs; d; d = d->tail)
      transDec(venv, tenv, d->head, level);
    exp = transExp(venv, tenv, a->u.let.body, level);
    S_endScope(tenv, 0);
    S_endScope(venv, 1);
    return exp;
  }
  case A_arrayExp: {
    Ty_ty array;
    struct expty size, init;
    array = actual_ty(S_look(tenv, a->u.array.typ));
    if (array->kind != Ty_array)
      EM_error(a->pos, "type-id(%s) must be declared as an array type",
               str_ty[array->kind]);
    size = transExp(venv, tenv, a->u.array.size, level);
    if (size.ty->kind != Ty_int)
      EM_error(a->pos, "size of array has non-integer value %s",
               str_ty[size.ty->kind]);
    init = transExp(venv, tenv, a->u.array.init, level);
    debug2("arrayExp: init.ty: %p->%s\n", init.ty, str_ty[init.ty->kind]);
    if (init.ty != array->u.array)
      EM_error(a->pos, "different array type (%p<-%p)", init.ty,
               array->u.array);
    return expTy(NULL, array);
  }
  default:
    assert(0);
  } // end switch(a->kind)
}

struct expty transVar(S_table venv, S_table tenv, A_var v, Tr_level level) {
  switch (v->kind) {
  case A_simpleVar: { // ID
    E_enventry x = S_look(venv, v->u.simple);
    if (x && x->kind == E_varEntry)
      return expTy(NULL, actual_ty(x->u.var.ty));
    EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
  }
  case A_fieldVar: { // lvalue DOT ID
    struct expty var;
    var = transVar(venv, tenv, v->u.field.var, level);
    debug2("fieldVar: var.ty: %p\n", var.ty);
    if (var.ty->kind != Ty_record)
      EM_error(v->u.field.var->pos, "reference base type (%s) is not a record",
               str_ty[var.ty->kind]);
    Ty_ty field_ty;
    if ((field_ty = findFieldInRecord(var.ty->u.record, v->u.field.sym)))
      return expTy(NULL, actual_ty(field_ty));
    EM_error(v->pos, "no member named '%s' in the record",
             S_name(v->u.field.sym));
  }
  case A_subscriptVar: { // ID LBRACK exp RBRACK
    struct expty array = transVar(venv, tenv, v->u.subscript.var, level);
    struct expty index = transExp(venv, tenv, v->u.subscript.exp, level);
    if (array.ty->kind != Ty_array)
      EM_error(v->u.subscript.var->pos, "subscripted value(%s) is not an array",
               str_ty[array.ty->kind]);
    if (index.ty->kind != Ty_int)
      EM_error(v->u.subscript.exp->pos, "array subscript(%s) is not an integer",
               str_ty[index.ty->kind]);
    return expTy(NULL, actual_ty(array.ty->u.array));
  }
  default:
    assert(0);
  }
}

void transDec(S_table venv, S_table tenv, A_dec d, Tr_level level) {
  switch (d->kind) {
  case A_functionDec: {
    // First, register all function head
    for (A_fundecList l = d->u.function; l; l = l->tail) {
      A_fundec f = l->head;
      Ty_ty resultTy = NULL;
      Ty_tyList formalTys = NULL;
      // check redefined function
      if (S_look(venv, f->name))
        debug("function with the same name \"%s\" is found!\n",
              S_name(f->name));
      // determine return type
      if (f->result == NULL) // procedure
        resultTy = Ty_Void();
      else // function
        resultTy = S_look(tenv, f->result);
      formalTys = makeFormalTyList(tenv, f->params);
      // push the formal list into env instead of pushing individually
      S_enter(venv, f->name, E_FunEntry(formalTys, resultTy));
    }
    // Second, translate function body
    for (A_fundecList l = d->u.function; l; l = l->tail) {
      E_enventry funEntry = S_look(venv, l->head->name);
      A_fundec f = l->head;
      debug2("functionDec: translating %s\n", S_name(f->name));

      S_beginScope(venv, 1);
      debug2("call Tr_newLevel\n");
      level = Tr_newLevel(level, Temp_newlabel(), genEscapeList(funEntry));
      {
        Ty_tyList t = funEntry->u.fun.formals;
        debug2("install function params\n\n");
#if DEBUG2
        TyList_print(t);
#endif
        for (A_fieldList L = f->params; L; L = L->tail, t = t->tail) {
          debug2("functionDec: %s->%s\n", S_name(L->head->name),
                 str_ty[t->head->kind]);
          S_enter(venv, L->head->name,
                  E_VarEntry(t->head, Tr_allocLocal(level, L->head->escape)));
        }
        debug2("install function params finished\n");
      }
      struct expty resultExp = transExp(venv, tenv, l->head->body, level);
      Ty_ty resultTy = actual_ty(resultExp.ty);
      Ty_ty funRetTy = actual_ty(funEntry->u.fun.result);
      if (resultTy->kind != funRetTy->kind) {
        if (resultTy->kind == Ty_nil && funRetTy->kind == Ty_record)
          ; // see MERGE.TIG 22-28
        else
          EM_error(
              f->pos,
              "incompatible returning %s from a function with result type %s",
              str_ty[resultTy->kind], str_ty[funRetTy->kind]);
      }
      S_endScope(venv, 1);
			
#if DEBUG
			debug("call Tr_printFormals on function %s(%s)\n", S_name(f->name), F_frameLable(level->frame));
			Tr_printFormals(Tr_formals(level));
#endif

      level = level->parent;
    }
    break;
  } // end case A_functionDec
  case A_varDec: {
    struct expty e = transExp(venv, tenv, d->u.var.init, level);
    if (d->u.var.typ) { // vardec -> var id : type-id := exp
      Ty_ty ty = actual_ty(S_look(tenv, d->u.var.typ));
      debug("varDec: %s : %s->type:%s\n", S_name(d->u.var.var),
            S_name(d->u.var.typ), str_ty[ty->kind]);
      debug("varDec: ty: %p e.ty: %p\n", ty, e.ty);
      if (ty->kind == Ty_record && e.ty->kind == Ty_nil) { // NULL
        S_enter(venv, d->u.var.var,
                E_VarEntry(ty, Tr_allocLocal(level, d->u.var.escape)));
        break;
      } else if (ty->kind != Ty_record && e.ty->kind == Ty_nil)
        EM_error(d->pos, "only record type can be assigned a nil value (%s)",
                 str_ty[ty->kind]);
      else if ((ty->kind == Ty_record || ty->kind == Ty_array) &&
               ty != actual_ty(e.ty))
        /* fprintf(stderr, "\nvarDec: NOT SURE\n\n"); */
        EM_error(d->pos, "different array/record types (%p<-%p)", ty,
                 actual_ty(e.ty));
      else if (ty->kind != e.ty->kind)
        EM_error(d->pos, "incompatible type (%s to %s)", str_ty[e.ty->kind],
                 str_ty[ty->kind]);
    } else { // vardec -> var id := exp
      if (e.ty->kind == Ty_nil)
        EM_error(d->pos,
                 "nil can only be used where the type can be determine");
    }
    S_enter(venv, d->u.var.var,
            E_VarEntry(e.ty, Tr_allocLocal(level, d->u.var.escape)));
    break;
  }
  case A_typeDec: {
    A_nametyList l;
    A_namety h;
    // declare the headers
    for (l = d->u.type; l; l = l->tail) {
      h = l->head;
      S_enter(tenv, h->name, Ty_Name(h->name, NULL));
    }
    for (l = d->u.type; l; l = l->tail) {
      h = l->head;
      Ty_ty real_ty = transTy(tenv, h->ty);
      ((Ty_ty)S_look(tenv, h->name))->u.name.ty = real_ty;
    }
#if DEBUG2
    for (l = d->u.type; l; l = l->tail) {
      h = l->head;
      if (h->ty->kind == A_recordTy) {
        debug("typeDec: record %s->%p: ", S_name(h->name),
              actual_ty(S_look(tenv, h->name)));
        for (Ty_fieldList l = actual_ty(S_look(tenv, h->name))->u.record; l;
             l = l->tail) {
          debug("%s: %s->%p; ", S_name(l->head->name),
                str_ty[actual_ty(l->head->ty)->kind], actual_ty(l->head->ty));
        }
        fprintf(stderr, "\n");
      } else {
        Ty_print(actual_ty(
            S_look(tenv, h->name))); // keep this to check for infinite loop
        fprintf(stderr, "\n");
      }
    }
#endif
    break;
  }
  default:
    assert(0);
  }
}

Ty_ty transTy(S_table tenv, A_ty a) {
  switch (a->kind) {
  case A_nameTy: {
    Ty_ty ty = S_look(tenv, a->u.name);
    if (ty)
      return ty;
    else
      EM_error(a->pos, "type %s is not defined", S_name(a->u.name));
  }
  case A_recordTy: {
    A_fieldList l = a->u.record;
    Ty_fieldList L, L_head = NULL;
    if (l) {
      L = Ty_FieldList(Ty_Field(l->head->name, S_look(tenv, l->head->typ)),
                       NULL);
      L_head = L;
      l = l->tail;
    }
    for (; l; l = l->tail, L = L->tail)
      L->tail = Ty_FieldList(
          Ty_Field(l->head->name, S_look(tenv, l->head->typ)), NULL);
    return Ty_Record(L_head);
  }
  case A_arrayTy: {
    Ty_ty ty = Ty_Array(actual_ty(S_look(tenv, a->u.array)));
    return ty;
  }
  default:
    assert(0);
  }
}

void SEM_transProg(A_exp exp) {
  transExp(E_base_venv(), E_base_tenv(), exp, Tr_outermost());
}

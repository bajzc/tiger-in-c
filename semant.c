/* semant.c */
#include <stdio.h>
#include <stdlib.h>

#include "absyn.h"
#include "env.h"
#include "errormsg.h"
#include "escape.h"
#include "semant.h"

#include "canon.h"
#include "printtree.h"
#include "symbol.h"
#include "translate.h"
#include "types.h"
#include "util.h"

static int breakLevel = 0;

struct expty {
  Tr_exp exp;
  Ty_ty ty;
};

static char str_ty[][12] = {"ty_record", "ty_nil",  "ty_int", "ty_string",
                            "ty_array",  "ty_name", "ty_void"};

/**
 * @param exp translated expression in intermediate code
 * @param ty Tiger-language type
 */
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
    EM_error((A_pos) {0}, "definition of recursive types is not completed");
  // avoid infinite loop
  Ty_ty ty_start = ty;
  while (ty->kind == Ty_name) {
    ty = ty->u.name.ty;
    if (ty == ty_start)
      EM_error((A_pos) {0}, "infinite loop of recursive type");
  }
  return ty;
}

static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params) {
  Ty_tyList l_head = NULL;
  if (params) {
    Ty_tyList l = Ty_TyList(actual_ty(S_look(tenv, params->head->typ)), NULL);
    l_head = l;
    params = params->tail;
    for (; params; params = params->tail, l = l->tail) {
      l->tail = Ty_TyList(actual_ty(S_look(tenv, params->head->typ)), NULL);
    }
  }
#if DEBUG2
  TyList_print(l_head);
  debug("\n");
#endif
  return l_head;
}

static Ty_ty findFieldInRecord(Ty_fieldList record, S_symbol sym) {
  // record can be empty
  // assert(record);
  assert(sym);
  for (Ty_fieldList h = record; h; h = h->tail) {
    if (h->head->name == sym) // `sym`s are unique ptr
      return actual_ty(h->head->ty);
  }
  return NULL;
}

// record.sym -> record[offset(sym)]
static int fieldOffset(Ty_fieldList record, S_symbol sym) {
  int i = 0;
  for (; record; record = record->tail) {
    if (record->head->name == sym) {
      debug("find field '%s' at [%d]\n", S_name(sym), i);
      return i;
    }
    i++;
  }
  assert(0);
}

static int fieldNumber(Ty_fieldList record) {
  int i = 0;
  for (; record; record = record->tail) {
    i++;
  }
  return i;
}

static int seqNumber(A_expList l) {
  int i = 0;
  for (; l; l = l->tail) {
    i++;
  }
  return i;
}

static int decNumber(A_decList decs) {
  int i = 0;
  for (; decs; decs = decs->tail) {
    i++;
  }
  return i;
}

struct expty transExp(S_table venv, S_table tenv, A_exp a, Tr_level level,
                      Temp_label break_label) {
  assert(a);
  switch (a->kind) {
    case A_varExp: {
      return transVar(venv, tenv, a->u.var, level);
    }
    case A_nilExp:
      return expTy(Tr_nilExp(), Ty_Nil());
    case A_intExp:
      return expTy(Tr_intExp(a->u.intt), Ty_Int());
    case A_stringExp:
      return expTy(Tr_stringExp(a->u.stringg), Ty_String());
    case A_callExp: {
      E_enventry funTy = S_look(venv, a->u.call.func);
      if (funTy && funTy->kind == E_funEntry) {
        Ty_tyList l = funTy->u.fun.formals;
        A_expList e = a->u.call.args;
        int argc = seqNumber(e);
        Tr_exp *argv = checked_malloc(sizeof(Tr_exp) * argc);
        int i = 0;
        while (e && l) {
          struct expty eTy = transExp(venv, tenv, e->head, level, break_label);
          if (eTy.ty->kind == Ty_nil && l->head->kind == Ty_record)
            ; // OK
          else if (eTy.ty->kind != l->head->kind)
            EM_error(e->head->pos,
                     "expected '%s' but argument is of "
                     "type '%s'",
                     str_ty[l->head->kind], str_ty[eTy.ty->kind]);
          else if(actual_ty(eTy.ty) != actual_ty(l->head)) {
            if (eTy.ty->kind == Ty_array)
              EM_error(e->head->pos,
                       "different array type as function "
                       "was defined %p:%p",
                       actual_ty(eTy.ty), actual_ty(l->head));
            if (eTy.ty->kind == Ty_record)
              EM_error(e->head->pos,
                       "different record type as function "
                       "was defined %p:%p",
                       actual_ty(eTy.ty), actual_ty(l->head));
            else
              EM_error(e->head->pos, "UNKNOWN type different");
          }
          e = e->tail;
          l = l->tail;
          argv[i++] = eTy.exp;
        }
        if (e)
          EM_error(a->pos, "passed more arguments than needed");
        if (l)
          EM_error(a->pos, "passed less arguments than needed");
        return expTy(Tr_callExp(funTy->u.fun.label, argv, argc, level,
                                funTy->u.fun.level),
                     actual_ty(funTy->u.fun.result));
      }
      EM_error(a->pos, "function '%s' not declared", S_name(a->u.call.func));
    }
    case A_opExp: {
      A_oper oper = a->u.op.oper;
      struct expty left =
          transExp(venv, tenv, a->u.op.left, level, break_label);
      struct expty right =
          transExp(venv, tenv, a->u.op.right, level, break_label);
      int l = left.ty->kind;
      int r = right.ty->kind;
      switch (oper) {
        case A_plusOp:
        case A_minusOp:
        case A_timesOp:
        case A_divideOp:
          if (l != Ty_int)
            EM_error(a->u.op.left->pos, "integer required");
          if (r != Ty_int)
            EM_error(a->u.op.right->pos, "integer required");
          return expTy(Tr_opExp(left.exp, oper, right.exp), Ty_Int());
        case A_eqOp:
        case A_neqOp:
        case A_ltOp:
        case A_leOp:
        case A_gtOp:
        case A_geOp:
          // tree.head <> nil
          // "abc" <> "def"
          // tree1 = tree2
          if ((l == Ty_record && r == Ty_nil) ||
              (l == Ty_nil && r == Ty_record))
            // comparison with NULL
            // TODO T_Cjump can handle null comparison
            return expTy(Tr_opExp(left.exp, oper, right.exp), Ty_Int());
          if (l != r)
            // no exception other than above
            EM_error(a->pos, "same type required for both sides");
          if (l == Ty_string) {
            // handle string comparison
            return expTy(Tr_eqExpString(left.exp, oper, right.exp), Ty_Int());
          }
          if (l != Ty_int && l != Ty_record && l != Ty_array)
            EM_error(a->pos,
                     "only integer, record, string and array can be compared "
                     "(%s <> %s)",
                     str_ty[l], str_ty[r]);
          return expTy(Tr_opExp(left.exp, oper, right.exp), Ty_Int());
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

      int fieldCount = fieldNumber(ty->u.record);
      Tr_exp *offsets = checked_malloc(sizeof(Tr_exp) * fieldCount);

      for (A_efieldList l = a->u.record.fields; l; l = l->tail) {
        Ty_ty fieldTy = findFieldInRecord(ty->u.record, l->head->name);
        if (fieldTy == NULL)
          EM_error(a->pos, "no member named '%s' in the record '%s'",
                   S_name(l->head->name), S_name(a->u.record.typ));
        struct expty eTy =
            transExp(venv, tenv, l->head->exp, level, break_label);
        if (fieldTy->kind == Ty_record && eTy.ty->kind == Ty_nil) {
          ; // OK
        } else if (eTy.ty != fieldTy)
          EM_error(a->pos, "incompatible type(%s->%s) when initial field '%s'",
                   str_ty[eTy.ty->kind], str_ty[fieldTy->kind],
                   S_name(l->head->name));
        offsets[fieldOffset(ty->u.record, l->head->name)] = eTy.exp;
      }
      return expTy(Tr_recordExp(offsets, fieldCount),
                   actual_ty(S_look(tenv, a->u.record.typ)));
    }
    case A_seqExp: {
      struct expty e;
      if (a->u.seq == NULL)
        EM_error(a->pos, "empty expression");
      int fieldCount = seqNumber(a->u.seq);
      Tr_exp *seqs = checked_malloc(sizeof(Tr_exp) * fieldCount);
      int i = 0;
      for (A_expList l = a->u.seq; l; l = l->tail) {
        e = transExp(venv, tenv, l->head, level, break_label);
        seqs[i++] = e.exp;
      }
      return expTy(Tr_seqExp(seqs, fieldCount), e.ty);
    }
    case A_assignExp: {
      struct expty lvalue = transVar(venv, tenv, a->u.assign.var, level);
      struct expty exp =
          transExp(venv, tenv, a->u.assign.exp, level, break_label);
      if (lvalue.ty->kind == Ty_record && exp.ty->kind == Ty_nil) {
        ; // OK
      } else if (lvalue.ty->kind != exp.ty->kind)
        EM_error(a->pos, "assign %s type to %s\n", str_ty[exp.ty->kind],
                 str_ty[lvalue.ty->kind]);
      return expTy(Tr_assignExp(lvalue.exp, exp.exp), Ty_Void());
    }
    case A_ifExp: {
      assert(a->u.iff.test);
      assert(a->u.iff.then);
      struct expty test =
          transExp(venv, tenv, a->u.iff.test, level, break_label);
      if (test.ty->kind != Ty_int)
        EM_error(a->u.iff.test->pos,
                 "result of condition should be an integer value (%s)",
                 str_ty[test.ty->kind]);
      struct expty then =
          transExp(venv, tenv, a->u.iff.then, level, break_label);
      if (a->u.iff.elsee) {
        // if-then-else
        struct expty elsee =
            transExp(venv, tenv, a->u.iff.elsee, level, break_label);
        if (then.ty->kind != elsee.ty->kind) {
          // see LANG MANUAL A.3.2.nil and MERGE.TIG:22-28
          if (then.ty->kind == Ty_nil && elsee.ty->kind == Ty_record)
            return expTy(Tr_ifExp(test.exp, then.exp, elsee.exp),
                         actual_ty(elsee.ty));
          if (then.ty->kind == Ty_record && elsee.ty->kind == Ty_nil)
            return expTy(Tr_ifExp(test.exp, then.exp, elsee.exp),
                         actual_ty(then.ty));
          EM_error(
              a->pos,
              "ifExp: expression of then(%s) and else(%s) does not have the "
              "same type",
              str_ty[then.ty->kind], str_ty[elsee.ty->kind]);
        }
        return expTy(Tr_ifExp(test.exp, then.exp, elsee.exp),
                     actual_ty(elsee.ty));
      } else {
        // if-then LANG MANUAL says should produce no value
        if (then.ty->kind != Ty_void)
          EM_error(a->u.iff.then->pos,
                   "expression of then-body(%s) must produce no value",
                   str_ty[then.ty->kind]);
        return expTy(Tr_ifExp(test.exp, then.exp, NULL), Ty_Void());
      }
      assert(0);
    }
    case A_whileExp: {
      assert(a->u.whilee.test);
      struct expty test =
          transExp(venv, tenv, a->u.whilee.test, level, break_label);
      if (test.ty->kind != Ty_int)
        EM_error(a->u.whilee.test->pos,
                 "result of condition should be an integer value (%s)",
                 str_ty[test.ty->kind]);
      beginBreakScope();
      Temp_label new_break = Temp_newlabel();
      debug("new break label '%s' created at line %d\n",
            Temp_labelstring(new_break), a->pos.first_line);
      struct expty body =
          transExp(venv, tenv, a->u.whilee.body, level, new_break);
      endBreakScope();
      if (body.ty->kind != Ty_void)
        EM_error(a->u.whilee.body->pos,
                 "expression of while-body(%s) must produce no value",
                 str_ty[body.ty->kind]);
      return expTy(Tr_whileExp(test.exp, body.exp, new_break), Ty_Void());
    }
    case A_forExp: {
      assert(a->u.forr.lo);
      assert(a->u.forr.hi);
      assert(a->u.forr.body);
      struct expty lo = transExp(venv, tenv, a->u.forr.lo, level, break_label);
      struct expty hi = transExp(venv, tenv, a->u.forr.hi, level, break_label);
      if (lo.ty->kind != Ty_int)
        EM_error(a->u.forr.lo->pos, "initial value(%s) should be an integer",
                 str_ty[lo.ty->kind]);
      if (hi.ty->kind != Ty_int)
        EM_error(a->u.forr.hi->pos, "upper bound(%s) should be an integer",
                 str_ty[hi.ty->kind]);
      Temp_label new_break = Temp_newlabel();
      S_beginScope(venv, 1);
      S_beginScope(tenv, 0);
      struct expty body;
      Tr_exp var;
      beginBreakScope();
      {
        // FIXME ensure not assign to var in this scope
        Tr_access var_acc = Tr_allocLocal(level, a->u.forr.escape);
        S_enter(venv, a->u.forr.var, E_VarEntry(lo.ty, var_acc));
        body = transExp(venv, tenv, a->u.forr.body, level, break_label);
        var = Tr_simpleVar(var_acc, level);
        if (body.ty->kind != Ty_void)
          EM_error(a->u.forr.body->pos,
                   "for-loop's body must produce no value (%s)",
                   str_ty[body.ty->kind]);
      }
      S_endScope(tenv, 0);
      S_endScope(venv, 1);
      endBreakScope();
      return expTy(Tr_forExp(body.exp, var, lo.exp, hi.exp, break_label),
                   Ty_Void());
    }
    case A_breakExp:
      if (breakLevel <= 0)
        EM_error(a->pos, "'break' statement not in loop");
      debug("break at line %d jump to label: %s\n", a->pos.first_line,
            Temp_labelstring(break_label));
      return expTy(Tr_breakExp(break_label), Ty_Void());
    case A_letExp: {
      S_beginScope(venv, 1);
      S_beginScope(tenv, 0);
      int dec_number = decNumber(a->u.let.decs);
      Tr_exp *decs = checked_malloc(sizeof(Tr_exp) * dec_number);
      int i = 0;
      for (A_decList d = a->u.let.decs; d; d = d->tail) {
        decs[i++] = transDec(venv, tenv, d->head, level, break_label);
      }
      struct expty exp =
          transExp(venv, tenv, a->u.let.body, level, break_label);
      S_endScope(tenv, 0);
      S_endScope(venv, 1);
      return expTy(Tr_letExp(decs, dec_number, exp.exp), exp.ty);
    }
    case A_arrayExp: {
      Ty_ty array = actual_ty(S_look(tenv, a->u.array.typ));
      if (array->kind != Ty_array)
        EM_error(a->pos, "type-id(%s) must be declared as an array type",
                 str_ty[array->kind]);
      struct expty size =
          transExp(venv, tenv, a->u.array.size, level, break_label);
      if (size.ty->kind != Ty_int)
        EM_error(a->pos, "size of array has non-integer value %s",
                 str_ty[size.ty->kind]);
      struct expty init =
          transExp(venv, tenv, a->u.array.init, level, break_label);
      debug2("arrayExp: init.ty: %p->%s\n", init.ty, str_ty[init.ty->kind]);
      if (init.ty != array->u.array)
        EM_error(a->pos, "different array type (%p<-%p)", init.ty,
                 array->u.array);
      return expTy(Tr_arrayExp(init.exp, size.exp->u.ex->u.CONST), array);
    }
    default:
      assert(0);
  } // end switch(a->kind)
}

struct expty transVar(S_table venv, S_table tenv, A_var v, Tr_level level) {
  assert(v);
  switch (v->kind) {
    case A_simpleVar: {
      // ID
      E_enventry x = S_look(venv, v->u.simple);
      debug("transVar: simple variable %s\n", S_name(v->u.simple));
      if (x && x->kind == E_varEntry)
        return expTy(Tr_simpleVar(x->u.var.access, level),
                     actual_ty(x->u.var.ty));
      EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
    }
    case A_fieldVar: {
      // lvalue DOT ID
      struct expty var = transVar(venv, tenv, v->u.field.var, level);
      debug2("fieldVar: var.ty: %p\n", var.ty);
      if (var.ty->kind != Ty_record)
        EM_error(v->u.field.var->pos,
                 "reference base type (%s) is not a record",
                 str_ty[var.ty->kind]);
      Ty_ty field_ty;
      if ((field_ty = findFieldInRecord(var.ty->u.record, v->u.field.sym))) {
        // print debug info
        debug("record info of record(%p):\n", actual_ty(var.ty));
#if DEBUG
        for (Ty_fieldList l = var.ty->u.record; l; l = l->tail) {
          fprintf(stderr, ".%s :: %s->%p; ", S_name(l->head->name),
                  str_ty[actual_ty(l->head->ty)->kind], actual_ty(l->head->ty));
        }
        fprintf(stderr, "\n");
#endif
        int offset = fieldOffset(var.ty->u.record, v->u.field.sym);
        return expTy(Tr_fieldVar(var.exp, offset), actual_ty(field_ty));
      } else
        EM_error(v->pos, "no member named '%s' in the record",
                 S_name(v->u.field.sym));
    }
    case A_subscriptVar: {
      // ID LBRACK exp RBRACK
      struct expty array = transVar(venv, tenv, v->u.subscript.var, level);
      struct expty index =
          transExp(venv, tenv, v->u.subscript.exp, level, NULL);
      if (array.ty->kind != Ty_array)
        EM_error(v->u.subscript.var->pos,
                 "subscripted value(%s) is not an array",
                 str_ty[array.ty->kind]);
      if (index.ty->kind != Ty_int)
        EM_error(v->u.subscript.exp->pos,
                 "array subscript(%s) is not an integer",
                 str_ty[index.ty->kind]);
      return expTy(Tr_subscriptVar(array.exp, index.exp),
                   actual_ty(array.ty->u.array));
    }
    default:
      assert(0);
  }
}

Tr_exp transDec(S_table venv, S_table tenv, A_dec d, Tr_level level,
                Temp_label break_label) {
  assert(d);
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
        Temp_label label = Temp_newlabel();
        S_enter(venv, f->name,
                E_FunEntry(Tr_newLevel(level, label, genEscapeList(formalTys)),
                           label, formalTys, resultTy));
      }
      // Second, translate function body
      for (A_fundecList l = d->u.function; l; l = l->tail) {
        E_enventry funEntry = S_look(venv, l->head->name);
        A_fundec f = l->head;
        debug2("functionDec: translating %s\n", S_name(f->name));

        S_beginScope(venv, 1);
        debug2("call Tr_newLevel\n");
        Tr_level fun_level = funEntry->u.fun.level;
        {
          Ty_tyList t = funEntry->u.fun.formals;
#if DEBUG2
          debug2("install function params\n\n");
          TyList_print(t);
#endif
          for (A_fieldList L = f->params; L; L = L->tail, t = t->tail) {
            debug2("functionDec: %s->%s\n", S_name(L->head->name),
                   str_ty[t->head->kind]);
            S_enter(venv, L->head->name,
                    E_VarEntry(t->head, Tr_allocLocal(fun_level, L->head->escape)));
          }
          debug2("install function params finished\n");
        }
        struct expty resultExp =
            transExp(venv, tenv, l->head->body, fun_level, break_label);
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
        debug("call Tr_printFormals on function '%s'(%s)\n", S_name(f->name),
              F_frameLabel(fun_level->frame));
        Tr_printFormals(Tr_formals(fun_level));
#endif

        Tr_procEntryExit(fun_level, resultExp.exp, Tr_formals(fun_level));
        level = level->parent;
      }
      return Tr_nilExp();
    } // end case A_functionDec
    case A_varDec: {
      struct expty e = transExp(venv, tenv, d->u.var.init, level, break_label);
      if (d->u.var.typ) {
        // vardec -> var id : type-id := exp
        Ty_ty ty = actual_ty(S_look(tenv, d->u.var.typ));
        debug("varDec: %s : %s->type:%s\n", S_name(d->u.var.var),
              S_name(d->u.var.typ), str_ty[ty->kind]);
        debug("varDec: ty: %p e.ty: %p\n", ty, e.ty);
        if (ty->kind == Ty_record && e.ty->kind == Ty_nil) {
          E_enventry var =
              E_VarEntry(ty, Tr_allocLocal(level, d->u.var.escape));
          // NULL
          S_enter(venv, d->u.var.var, var);
          return Tr_assignExp(Tr_simpleVar(var->u.var.access, level), e.exp);
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
      } else {
        // vardec -> var id := exp
        if (e.ty->kind == Ty_nil)
          EM_error(d->pos,
                   "nil can only be used where the type can be determine");
      }
      E_enventry var = E_VarEntry(e.ty, Tr_allocLocal(level, d->u.var.escape));
      S_enter(venv, d->u.var.var, var);
      return Tr_assignExp(Tr_simpleVar(var->u.var.access, level), e.exp);
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
        ((Ty_ty) S_look(tenv, h->name))->u.name.ty = real_ty;
      }
#if DEBUG2
      for (l = d->u.type; l; l = l->tail) {
        h = l->head;
        if (h->ty->kind == A_recordTy) {
          fprintf(stderr,"typeDec: record %s->%p: ", S_name(h->name),
                actual_ty(S_look(tenv, h->name)));
          for (Ty_fieldList fl = actual_ty(S_look(tenv, h->name))->u.record; fl;
               fl = fl->tail) {
            fprintf(stderr,"%s: %s->%p; ", S_name(fl->head->name),
                  str_ty[actual_ty(fl->head->ty)->kind],
                  actual_ty(fl->head->ty));
          }
          fprintf(stderr, "\n");
        } else {
          Ty_print(actual_ty(
              S_look(tenv, h->name))); // keep this to check for infinite loop
          fprintf(stderr, "\n");
        }
      }
#endif
      return Tr_nilExp();
    }
    default:
      assert(0);
  }
}

Ty_ty transTy(S_table tenv, A_ty a) {
  assert(a);
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
      Ty_fieldList L_head = NULL;
      if (l) {
        Ty_fieldList L = Ty_FieldList(
            Ty_Field(l->head->name, S_look(tenv, l->head->typ)), NULL);
        L_head = L;
        l = l->tail;
        for (; l; l = l->tail, L = L->tail)
          L->tail = Ty_FieldList(
              Ty_Field(l->head->name, S_look(tenv, l->head->typ)), NULL);
      }
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
  struct expty res = transExp(E_base_venv(), E_base_tenv(), exp, Tr_outermost(), NULL);
  debug("call Tr_printFormals on outermost evn(%s)\n",
        Temp_labelstring(Tr_outermost()->name));
  Tr_printFormals(Tr_formals(Tr_outermost()));
  printf("\nOriginal:\n");
  printStmList(stdout, T_StmList(unNx(res.exp), NULL));
  printf("\nLinearized:\n");
  printStmList(stdout, C_linearize(unNx(res.exp)));
}

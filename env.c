/* env.c */

#include <stdio.h>

#include "env.h"
#include "symbol.h"
#include "table.h"
#include "types.h"
#include "util.h"

TAB_table E_funLabel2funEntry = NULL;

S_table E_venv, E_tenv;

E_enventry E_VarEntry(Ty_ty ty, Tr_access access) {
  E_enventry entry = checked_malloc(sizeof(*entry));
  entry->kind = E_varEntry;
  entry->u.var.ty = ty;
  entry->u.var.access = access;
  return entry;
}

E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals,
                      Ty_ty result) {
  E_enventry entry = checked_malloc(sizeof(*entry));
  entry->kind = E_funEntry;
  entry->u.fun.level = level;
  entry->u.fun.label = label;
  entry->u.fun.formals = formals;
  entry->u.fun.result = result;
  return entry;
}

S_table E_base_tenv(void) {
  S_table tenv = S_empty();
  S_enter(tenv, S_Symbol("int"), Ty_Int());
  S_enter(tenv, S_Symbol("string"), Ty_String());
  return tenv;
}

static E_enventry E_libFun(Tr_level funLevel, Ty_tyList formals, Ty_ty resulty,
                           char *name) {
  Temp_label t = Temp_namedlabel(name);
  E_enventry f = E_FunEntry(funLevel, t, formals, resulty);
  TAB_enter(E_funLabel2funEntry, t, f);
  return f;
}

#define T(name) (Temp_namedlabel(name))
#define U(head, tail) (U_BoolList(head, tail))
#define L(head, tail) (Ty_TyList(head, tail))

S_table E_base_venv(void) {
  if (E_funLabel2funEntry == NULL)
    E_funLabel2funEntry = TAB_empty();
  S_table venv = S_empty();
  S_enter(venv, S_Symbol("printInt"),
          E_libFun(Tr_libFunLevel(T("printInt"), U(0, NULL)), L(Ty_Int(), NULL),
                   Ty_Void(), "printInt"));
  S_enter(venv, S_Symbol("print"),
          E_libFun(Tr_libFunLevel(T("print"), U(0, NULL)), L(Ty_String(), NULL),
                   Ty_Void(), "print"));
  S_enter(venv, S_Symbol("flush"),
          E_libFun(Tr_libFunLevel(T("flush"), NULL), NULL, Ty_Void(), "flush"));
  S_enter(venv, S_Symbol("getchar"),
          E_libFun(Tr_libFunLevel(T("getchar"), NULL), NULL, Ty_String(),
                   "getchar"));
  S_enter(venv, S_Symbol("ord"),
          E_libFun(Tr_libFunLevel(T("ord"), U(0, NULL)), L(Ty_String(), NULL),
                   Ty_Int(), "ord"));
  S_enter(venv, S_Symbol("chr"),
          E_libFun(Tr_libFunLevel(T("chr"), U(0, NULL)), L(Ty_Int(), NULL),
                   Ty_String(), "chr"));
  S_enter(venv, S_Symbol("size"),
          E_libFun(Tr_libFunLevel(T("size"), U(0, NULL)), L(Ty_String(), NULL),
                   Ty_Int(), "size"));
  S_enter(venv, S_Symbol("substring"),
          E_libFun(Tr_libFunLevel(T("substring"), U(0, U(0, U(0, NULL)))),
                   L(Ty_String(), L(Ty_Int(), L(Ty_Int(), NULL))), Ty_String(),
                   "substring"));
  S_enter(venv, S_Symbol("concat"),
          E_libFun(Tr_libFunLevel(T("concat"), U(0, U(0, NULL))),
                   L(Ty_String(), L(Ty_String(), NULL)), Ty_String(),
                   "concat"));
  S_enter(venv, S_Symbol("not"),
          E_libFun(Tr_libFunLevel(T("not"), U(0, NULL)), L(Ty_Int(), NULL),
                   Ty_Int(), "not"));
  S_enter(venv, S_Symbol("exit"),
          E_libFun(Tr_libFunLevel(T("exit"), U(0, NULL)), L(Ty_Int(), NULL),
                   Ty_Void(), "exit"));
  S_enter(venv, S_Symbol("initArray"),
          E_libFun(Tr_libFunLevel(T("initArray"), U(0, U(0, NULL))),
                                  L(Ty_Int(), L(Ty_Int(), NULL)), Ty_Int(),
                                  "initArray"));
  return venv;
}

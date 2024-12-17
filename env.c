/* env.c */

#include <stdio.h>

#include "env.h"
#include "symbol.h"
#include "types.h"
#include "util.h"

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

E_enventry E_base_fun(Ty_tyList formals, Ty_ty resulty) {
  return E_FunEntry(Tr_outermost(), Temp_newlabel(), formals, resulty);
}

S_table E_base_venv(void) {
  S_table venv = S_empty();
  S_enter(venv, S_Symbol("print"),
          E_base_fun(Ty_TyList(Ty_String(), NULL), Ty_Void()));
  S_enter(venv, S_Symbol("flush"), E_base_fun(NULL, Ty_Void()));
  S_enter(venv, S_Symbol("getchar"), E_base_fun(NULL, Ty_String()));
  S_enter(venv, S_Symbol("ord"),
          E_base_fun(Ty_TyList(Ty_String(), NULL), Ty_Int()));
  S_enter(venv, S_Symbol("chr"),
          E_base_fun(Ty_TyList(Ty_Int(), NULL), Ty_String()));
  S_enter(venv, S_Symbol("size"),
          E_base_fun(Ty_TyList(Ty_String(), NULL), Ty_Int()));
  S_enter(venv, S_Symbol("substring"),
          E_base_fun(Ty_TyList(Ty_String(),
                               Ty_TyList(Ty_Int(), Ty_TyList(Ty_Int(), NULL))),
                     Ty_String()));
  S_enter(venv, S_Symbol("concat"),
          E_base_fun(Ty_TyList(Ty_String(), Ty_TyList(Ty_String(), NULL)),
                     Ty_String()));
  S_enter(venv, S_Symbol("not"),
          E_base_fun(Ty_TyList(Ty_Int(), NULL), Ty_Int()));
  S_enter(venv, S_Symbol("exit"),
          E_base_fun(Ty_TyList(Ty_Int(), NULL), Ty_Void()));
  return venv;
}

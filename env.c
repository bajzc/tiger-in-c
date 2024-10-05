/* env.c */

#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "types.h"
#include "env.h"

E_enventry E_VarEntry(Ty_ty ty) {
  E_enventry entry = checked_malloc(sizeof(*entry));
  entry->kind = E_varEntry;
  entry->u.var.ty = ty;
  return entry;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result) {
  E_enventry entry = checked_malloc(sizeof(*entry));
  entry->kind = E_funEntry;
  entry->u.fun.formals = formals;
  entry->u.fun.result = result;
  return entry;
}

S_table E_base_tenv(void){
  S_table tenv = S_empty();
  S_enter(tenv, S_Symbol("int"), Ty_Int());
  S_enter(tenv, S_Symbol("string"), Ty_String());
  return tenv;
}

S_table E_base_venv(void) {
  // TODO fill in built-in functions
  S_table tenv = S_empty();
  S_enter(tenv, S_Symbol("print"),
          E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Void()));
  S_enter(tenv, S_Symbol("flush"), E_FunEntry(NULL, Ty_Void()));
  S_enter(tenv, S_Symbol("getchar"), E_FunEntry(NULL, Ty_String()));
  S_enter(tenv, S_Symbol("ord"),
          E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int()));
  S_enter(tenv, S_Symbol("chr"),
          E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_String()));
  S_enter(tenv, S_Symbol("size"),
          E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int()));
  S_enter(tenv, S_Symbol("substring"),
          E_FunEntry(Ty_TyList(Ty_String(),
                               Ty_TyList(Ty_Int(), Ty_TyList(Ty_Int(), NULL))),
                     Ty_String()));
  S_enter(tenv, S_Symbol("concat"),
          E_FunEntry(Ty_TyList(Ty_String(), Ty_TyList(Ty_String(), NULL)),
                     Ty_String()));
  S_enter(tenv, S_Symbol("not"),
          E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Int()));
  S_enter(tenv, S_Symbol("exit"), E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Void()));
  return tenv;
}

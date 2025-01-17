/* env.h */

#ifndef ENV_H
#define ENV_H

#include "symbol.h"
#include "translate.h"
#include "types.h"

typedef struct E_enventry_ *E_enventry;

struct E_enventry_ {
  enum { E_varEntry, E_funEntry } kind;
  union {
    struct {
      Tr_access access;
      Ty_ty ty;
    } var;
    struct {
      Tr_level level;
      Temp_label label;
      Ty_tyList formals;
      Ty_ty result;
    } fun;
  } u;
};

E_enventry E_VarEntry(Ty_ty ty, Tr_access access);
E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals,
                      Ty_ty result);

S_table E_base_tenv(void); // Ty_ty environment
S_table E_base_venv(void); // E_enventry environment

extern S_table E_venv, E_tenv;
extern TAB_table E_funLabel2funEntry;

#endif

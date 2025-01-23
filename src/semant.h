/* semant.h */
#ifndef SEMANT_H
#define SEMANT_H
#include "absyn.h"
#include "translate.h"
#include "types.h"
extern TAB_table SEM_funLabel2funEntry; // Map<Temp_label, E_enventry>

F_fragList SEM_transProg(A_exp exp);

struct expty transVar(S_table venv, S_table tenv, A_var v, Tr_level level);
struct expty transExp(S_table venv, S_table tenv, A_exp a, Tr_level level,
                      Temp_label break_label);
Tr_exp transDec(S_table venv, S_table tenv, A_dec d, Tr_level level,
                Temp_label break_label);
Ty_ty transTy(S_table tenv, A_ty a);
#endif

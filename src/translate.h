/* translate.h */
#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "absyn.h"
#include "frame.h"
#include "temp.h"

typedef struct Tr_access_ *Tr_access;
typedef struct Tr_exp_ *Tr_exp;
typedef struct Tr_accessList_ *Tr_accessList;
typedef struct Tr_fieldOffset_ *Tr_fieldOffset;
typedef struct Tr_level_ *Tr_level;
typedef struct patchList_ *patchList;

struct Tr_accessList_ {
  Tr_access head;
  Tr_accessList tail;
};

struct Tr_level_ {
  F_frame frame;
  Tr_level parent;
  Temp_label name;
  U_boolList formals; // escape or not
};

struct Cx {
  patchList trues;
  patchList falses;
  T_stm stm;
};

struct Tr_exp_ {
  enum { Tr_ex, Tr_nx, Tr_cx } kind;
  union {
    // an expression
    T_exp ex;
    // no result
    T_stm nx;
    // condition
    struct Cx cx;
  } u;
};

struct patchList_ {
  Temp_label *head;
  patchList tail;
};

struct Tr_access_ {
  Tr_level level;
  F_access access;
};

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);
Tr_access Tr_Access(Tr_level level,  F_access access);
Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals_with_static_link(Tr_level level);
Tr_level Tr_libFunLevel(Temp_label name, U_boolList formals);
Tr_access Tr_allocLocal(Tr_level level, bool escape, bool isPointer);
void Tr_printFormals(Tr_accessList formals);
Tr_exp Tr_eqExpString(Tr_exp l, A_oper op, Tr_exp r);
Tr_exp Tr_recordExp(Tr_exp *exps, int size, Ty_ty *types);
Tr_exp Tr_arrayExp(Tr_exp init, Tr_exp size, bool isPointer);
Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp body, Temp_label done);
Tr_exp Tr_breakExp(Temp_label done);
Tr_exp Tr_forExp(Tr_exp body, Tr_exp var, Tr_exp lo, Tr_exp hi,
                 Temp_label done);
Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp exp);
Tr_exp Tr_seqExp(Tr_exp *seqs, int size);
Tr_exp Tr_callExp(Temp_label name, Tr_exp *argv, int argc,
                  Tr_level caller_level, Tr_level callee_level,
                  bool isExternCall);
Tr_exp Tr_letExp(Tr_exp *decs, int size, Tr_exp body);

T_stm unNx(Tr_exp e);

Tr_exp Tr_simpleVar(Tr_access, Tr_level);
Tr_exp Tr_fieldVar(Tr_exp record, int offset);
Tr_exp Tr_subscriptVar(Tr_exp arr, Tr_exp index);
Tr_exp Tr_nilExp(void);
Tr_exp Tr_intExp(int a);
Tr_exp Tr_stringExp(string s);
Tr_exp Tr_opExp(Tr_exp l, A_oper op, Tr_exp r);

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee);

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals, bool return_flag);
F_fragList Tr_getResult(void);
Tr_accessList Tr_reverseList(Tr_accessList list);
#endif

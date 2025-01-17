#ifndef CODEGEN_H
#define CODEGEN_H

#include "assem.h"
#include "frame.h"
#include "set.h"

AS_instrList F_codegen(Set stmt2last_instr, F_frame f, T_stmList stmList);

struct stmt_instr {
  // TODO refactor this
  AS_instr last;
};

#endif // CODEGEN_H

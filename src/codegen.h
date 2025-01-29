#ifndef CODEGEN_H
#define CODEGEN_H

#include "assem.h"
#include "frame.h"
#include "set.h"

AS_instrList F_codegen(Set last_instr, F_frame f, T_stmList stmList);
extern TAB_table AS_ptrMapTable;

#endif // CODEGEN_H

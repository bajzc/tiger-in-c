/* frame.h */

#ifndef FRAME_H
#define FRAME_H

#include "types.h"
#include "assem.h"
#include "table.h"
#include "temp.h"
#include "tree.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;

typedef struct F_accessList_ *F_accessList;
struct F_accessList_ {
  F_access head;
  F_accessList tail;
};

typedef struct F_frag_ *F_frag;
struct F_frag_ {
  enum { F_stringFrag, F_procFrag } kind;
  union {
    struct {
      Temp_label label;
      string str;
    } stringg;
    struct {
      T_stm body;
      F_frame frame;
    } proc;
  } u;
};

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ {
  F_frag head;
  F_fragList tail;
};

extern const int F_wordSize;
extern Temp_map F_tempMap;
extern const int F_numGPR;
extern Temp_map F_reg2colorscheme;
extern Temp_map F_reg2color;
extern Set F_regTemp; // Set<Temp_temp>
extern Set F_regString; // Set<String> aka color
extern TAB_table F_color2reg; // Map<String, Temp_temp>


F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);
F_fragList F_FragList(F_frag head, F_fragList tail);

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);

F_accessList F_formals(F_frame f);
F_accessList F_locals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape, bool isPointer);

void F_printAccess(F_access f);
string F_frameLabel(F_frame f);
Temp_temp F_FP(void);
Temp_temp F_RV(void);
Temp_temp F_SP(void);
T_exp F_Exp(F_access acc, T_exp framePtr);
int F_isInReg(F_access a);
Temp_tempList F_args(void);
Temp_tempList F_callerSaves(void);

// call external helper function, eg. `initArray`
T_exp F_externalCall(string s, T_expList args);

T_stm F_procEntryExit1(F_frame frame, T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList body, F_frame frame, Set last_instr);
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);

F_accessList F_reverseList(F_accessList list);

#endif

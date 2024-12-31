/* RISC-V frame implementation */

// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc

#include "assem.h"
#include "frame.h"
#include "util.h"

#define ARG_IN_REG 8 // a0-a7
const int F_wordSize = 4;

struct F_access_ {
  enum { inFrame, inReg } kind;
  union {
    int offset;
    Temp_temp reg;
  } u;
};

struct F_frame_ {
  Temp_label frame_label;
  int stack_size; // the number of locals allocated so far
  F_accessList formals_list; // the locations of all the formals
  F_accessList locals_list;
};

/* |  ..       ..  |                  */
/* |  ARGUMENT 2   | <- incoming arguments */
/* |  ARGUMENT 1   |                  */
/* |  STATIC LINKS | <- FRAME POINTER */
/* ---------------------------------- */
/* |  LOCAL VAR    | <- current frame */
/* |  ..    ..     |                  */
/* |  return addr  |                  */
/* | temp save reg |                  */
/* -----------------                  */
/* |  ARGUMENT m   |                  */
/* |  ARGUMENT ..  |                  */
/* |  ARGUMENT 1   |                  */
/* |  STATIC LINKS | <- STACK POINTER */
/* ---------------------------------- */
/* |               | <- next frame    */

static Temp_temp F_fp = NULL;
Temp_temp F_FP(void) {
  if (F_fp == NULL) {
    F_fp = Temp_newtemp();
  }
  return F_fp;
}

static Temp_temp F_rv = NULL;
Temp_temp F_RV(void) {
  if (F_rv == NULL) {
    F_rv = Temp_newtemp();
  }
  return F_rv;
}

T_exp F_Exp(F_access acc, T_exp framePtr) {
  if (acc->kind == inReg) {
    return T_Temp(acc->u.reg);
  }
  return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
}

static F_access InFrame(int offset) {
  F_access f = checked_malloc(sizeof(*f));
  f->kind = inFrame;
  f->u.offset = offset;
  return f;
}

static F_access InReg(Temp_temp reg) {
  F_access r = checked_malloc(sizeof(*r));
  r->kind = inReg;
  r->u.reg = reg;
  return r;
}

F_access F_allocFormals(F_frame f, bool escape) {
  F_access ret = NULL;
  assert(f);
  if (escape) {
    debug("%s: call InFrame(%d)\n", Temp_labelstring(f->frame_label),
          f->stack_size * F_wordSize * -1);
    ret = InFrame(f->stack_size * F_wordSize * -1);
    f->stack_size += 1;
  } else
    ret = InReg(Temp_newtemp());
  F_accessList l = checked_malloc(sizeof(*l));
  l->head = ret;
  l->tail = f->formals_list;
  f->formals_list = l;
  return ret;
}

F_access F_allocLocal(F_frame f, bool escape) {
  F_access ret = NULL;
  assert(f);
  if (escape) {
    debug("%s: call InFrame(%d)\n", Temp_labelstring(f->frame_label),
          f->stack_size * F_wordSize * -1);
    ret = InFrame(f->stack_size * F_wordSize * -1);
    f->stack_size += 1;
  } else
    ret = InReg(Temp_newtemp());
  F_accessList l = checked_malloc(sizeof(*l));
  l->head = ret;
  l->tail = f->locals_list;
  f->locals_list = l;
  return ret;
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
  debug("'%s'\n", Temp_labelstring(name));
  F_frame frame = checked_malloc(sizeof(*frame));
  frame->frame_label = name;
  frame->stack_size = 0;
  bool escape;
  frame->formals_list = NULL;
  frame->locals_list = NULL;

  if (formals) { // the static links
    escape = formals->head;
    F_allocFormals(frame, escape);
    assert(frame->formals_list->head->kind == inFrame);
    debug("%s: installed static links\n", Temp_labelstring(name));
    formals = formals->tail;
  } else { // the global stack
    debug("%s: init global frame\n", Temp_labelstring(name));
    return frame;
  }

  while (formals) {
    escape = formals->head;
    F_allocFormals(frame, escape); // TODO how to make sure they are in a0-a7?
    debug("%s: install new param (escape=%d)\n", Temp_labelstring(name),
          escape);
    formals = formals->tail;
  }

  debug("%s\n", Temp_labelstring(name));
  return frame;
}

Temp_label F_name(F_frame f) { return f->frame_label; }

F_accessList F_formals(F_frame f) { return f->formals_list; }

void F_printAccess(F_access access) {
  assert(access);
  if (access->kind == inFrame) {
    debug("Frame[%d]\n", access->u.offset);
  } else {
    debug("Temp Reg[%d]\n", access->u.reg->num);
  }
}

string F_frameLabel(F_frame f) { return Temp_labelstring(f->frame_label); }

T_exp F_externalCall(string s, T_expList args) {
  // TODO adjust for underscore label when linking
  return T_Call(T_Name(Temp_namedlabel(s)), args);
}

F_frag F_StringFrag(Temp_label lab, string str) {
  F_frag f = checked_malloc(sizeof(*f));
  f->kind = F_stringFrag;
  f->u.stringg.label = lab;
  f->u.stringg.str = str;
  return f;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
  F_frag f = checked_malloc(sizeof(*f));
  f->kind = F_procFrag;
  f->u.proc.body = body;
  f->u.proc.frame = frame;
  return f;
}

F_fragList F_FragList(F_frag head, F_fragList tail) {
  F_fragList l = checked_malloc(sizeof(*l));
  l->head = head;
  l->tail = tail;
  return l;
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm) { return stm; }

#define DECLARE_REG(vname) static Temp_temp vname;
#define INIT_REG(vname, mname) vname = Temp_newtemp();\
  Temp_enter(F_tempMap, vname, #mname);
// #define INIT_REG_L(vname, mname, i, ...) __VA_OPT__( INIT_REG_HELPER (vname, mname, __VA_ARGS__); )\
//   vname##i = Temp_newtemp();\
//   Temp_enter(F_tempMap, vname##i, #mname#i);
// #define A(...) INIT_REG_L(__VA_ARGS__)
// #define INIT_REG_HELPER(...) A(__VA_ARGS__)

// TODO macro for adding list of regs

DECLARE_REG(ZERO)
DECLARE_REG(RA)
DECLARE_REG(SP)
DECLARE_REG(FP)
DECLARE_REG(RET0)
DECLARE_REG(RET1)
DECLARE_REG(ARG0)
DECLARE_REG(ARG1)
DECLARE_REG(ARG2)
DECLARE_REG(ARG3)
DECLARE_REG(ARG4)
DECLARE_REG(ARG5)
DECLARE_REG(S0)
DECLARE_REG(S1)
DECLARE_REG(S2)
DECLARE_REG(S3)
DECLARE_REG(S4)
DECLARE_REG(S5)
DECLARE_REG(S6)
DECLARE_REG(S7)
DECLARE_REG(S8)
DECLARE_REG(S9)
DECLARE_REG(S10)
DECLARE_REG(S11)
static Temp_tempList calleeSaves = Temp_tempList();

static void initRegMap() {
  INIT_REG(ZERO, zero);
  INIT_REG(RA, ra);
  INIT_REG(SP, sp);
  INIT_REG(FP, fp);

  // Caller saved
  INIT_REG(RET0, a0);
  INIT_REG(RET1, a1);
  INIT_REG(ARG0, a2);
  INIT_REG(ARG1, a3);
  INIT_REG(ARG2, a4);
  INIT_REG(ARG3, a5);
  INIT_REG(ARG4, a6);
  INIT_REG(ARG5, a7);

  // Callee
  INIT_REG(S0, s0);
  INIT_REG(S1, s1);
  INIT_REG(S2, s2);
  INIT_REG(S3, s3);
  INIT_REG(S4, s4);
  INIT_REG(S5, s5);
  INIT_REG(S6, s6);
  INIT_REG(S7, s7);
  INIT_REG(S8, s8);
  INIT_REG(S9, s9);
  INIT_REG(S10, s10);
  INIT_REG(S11, s11);
}

static Temp_tempList returnSink = NULL;
AS_instrList F_procEntryExit2(AS_instrList body) {
  if (!returnSink) returnSink =
  Temp_TempList(ZERO, Temp_TempList(RA,
  Temp_TempList(SP, calleeSaves)));
  return AS_splice(body, AS_InstrList(
  AS_Oper("", NULL, returnSink, NULL), NULL));
}
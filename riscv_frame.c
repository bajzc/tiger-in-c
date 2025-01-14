/* RISC-V frame implementation */

// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc

#include "assem.h"
#include "frame.h"
#include "set.h"
#include "table.h"
#include "util.h"

#define L(h, t) Temp_TempList((Temp_temp) h, (Temp_tempList) t)
#define ARG_IN_REG 7 // a1-a7
const int F_wordSize = 4; // target machine is RV32
const int F_numGPR = 26; // General Purpose Registers: 32 - 6
Temp_map F_tempMap;
// used for generate graphviz graph
Temp_map F_reg2colorscheme;
Temp_map F_reg2color;
Set F_regTemp; // Set<Temp_temp>
Set F_regString; // Set<String> aka color
TAB_table F_color2reg; // Map<String, Temp_temp>

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
  }

  while (formals) {
    escape = formals->head;
    F_allocFormals(frame, escape);
    debug("%s: install new param (escape=%d)\n", Temp_labelstring(name),
          escape);
    formals = formals->tail;
  }

  debug("finish %s\n", Temp_labelstring(name));
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

#define DECLARE_REG(vname) static Temp_temp vname;
#define INIT_REG(vname, mname, color_schemes, color)                           \
  vname = Temp_newtemp();                                                      \
  Temp_enter(F_tempMap, vname, #mname);                                        \
  Temp_enter(F_reg2colorscheme, vname, (#color_schemes));                      \
  TAB_enterString(F_color2reg, (#mname), vname);                               \
  Temp_enter(F_reg2color, vname, #color);                                      \
  SET_insert(F_regString, #mname);                                             \
  SET_insert(F_regTemp, vname);

DECLARE_REG(ZERO)
DECLARE_REG(RA)
DECLARE_REG(SP)
DECLARE_REG(FP)
DECLARE_REG(A0)
DECLARE_REG(A1)
DECLARE_REG(A2)
DECLARE_REG(A3)
DECLARE_REG(A4)
DECLARE_REG(A5)
DECLARE_REG(A6)
DECLARE_REG(A7)
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
DECLARE_REG(T0)
DECLARE_REG(T1)
DECLARE_REG(T2)
DECLARE_REG(T3)
DECLARE_REG(T4)
DECLARE_REG(T5)
DECLARE_REG(T6)
static Temp_tempList calleeSaves;
static Temp_tempList callerSaves;
static Temp_tempList argRegs;
static Temp_tempList specialRegs;

static void initRegMap() {
  if (ZERO != NULL)
    return;
  F_tempMap = Temp_empty();
  F_reg2colorscheme = Temp_empty();
  F_reg2color = Temp_empty();
  F_regString = SET_empty((int (*)(void *, void *)) strcmp);
  F_regTemp = SET_empty(SET_default_cmp);
  F_color2reg = TAB_empty();
  // INIT_REG(ZERO, zero, x11, gold);
  INIT_REG(RA, ra, x11, gold3);
  INIT_REG(SP, sp, x11, gold4);
  INIT_REG(FP, s0, x11, goldenrod);
  ZERO = Temp_newtemp();
  // SP = Temp_newtemp();
  // FP = Temp_newtemp();
  // RA = Temp_newtemp();

  // Caller saved
  INIT_REG(A0, a0, x11, red);
  INIT_REG(A1, a1, x11, green);
  INIT_REG(A2, a2, x11, blue);
  INIT_REG(A3, a3, x11, yellow);
  INIT_REG(A4, a4, x11, cyan);
  INIT_REG(A5, a5, x11, magenta);
  INIT_REG(A6, a6, x11, orange);
  INIT_REG(A7, a7, x11, pink);
  INIT_REG(T0, t0, x11, purple);
  INIT_REG(T1, t1, x11, brown);
  INIT_REG(T2, t2, x11, lightblue);
  INIT_REG(T3, t3, x11, darkgreen);
  INIT_REG(T4, t4, x11, gray);
  INIT_REG(T5, t5, x11, lightgray);
  INIT_REG(T6, t6, SVG, darksalmon);

  // Callee
  INIT_REG(S1, s1, x11, navy);
  INIT_REG(S2, s2, x11, teal);
  INIT_REG(S3, s3, x11, olive);
  INIT_REG(S4, s4, x11, gold);
  INIT_REG(S5, s5, x11, lime);
  INIT_REG(S6, s6, x11, maroon);
  INIT_REG(S7, s7, x11, violet);
  INIT_REG(S8, s8, x11, aquamarine);
  INIT_REG(S9, s9, x11, indigo);
  INIT_REG(S10, s10, x11, chartreuse);
  INIT_REG(S11, s11, x11, turquoise);

  specialRegs = Temp_TempList(
      ZERO,
      Temp_TempList(
          RA, Temp_TempList(SP, Temp_TempList(FP, Temp_TempList(A0, NULL)))));

  callerSaves = Temp_TempList(
      T0, Temp_TempList(
              T1, Temp_TempList(
                      T2, Temp_TempList(
                              T3, Temp_TempList(
                                      T4, Temp_TempList(
                                              T5, Temp_TempList(T6, NULL)))))));

  argRegs = Temp_TempList(
      A1, Temp_TempList(
              A2, Temp_TempList(
                      A3, Temp_TempList(
                              A4, Temp_TempList(
                                      A5, Temp_TempList(
                                              A6, Temp_TempList(A7, NULL)))))));

  calleeSaves = Temp_TempList(
      S1,
      Temp_TempList(
          S2,
          Temp_TempList(
              S3,
              Temp_TempList(
                  S4,
                  Temp_TempList(
                      S5,
                      Temp_TempList(
                          S6,
                          Temp_TempList(
                              S7,
                              Temp_TempList(
                                  S8, Temp_TempList(
                                          S9, Temp_TempList(
                                                  S10, Temp_TempList(
                                                           S11, NULL)))))))))));
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm) { return stm; }

static Temp_tempList returnSink = NULL;
AS_instrList F_procEntryExit2(AS_instrList body, F_frame frame) {
  if (!returnSink)
    returnSink =
        Temp_TempList(A0, Temp_TempList(RA, Temp_TempList(SP, calleeSaves)));
  // FIXME
  // copy stack pointer to frame pointer at function entry and restore it when
  // exit
  int frameSize = frame->stack_size * F_wordSize;
  char buf[80];
  AS_instrList insert_entry = NULL;
  assert(body->head->kind == I_LABEL);
  snprintf(buf, 80, "addi `d0, `s0, -%d # alloc stack space", frameSize);
  insert_entry = AS_InstrList(
      body->head, AS_InstrList(AS_Oper("mv `d0, `s0 # update frame pointer",
                                       L(FP, NULL), L(SP, NULL), NULL),
                               AS_InstrList(AS_Oper(STRDUP(buf), L(SP, NULL),
                                                    L(SP, NULL), NULL),
                                            body->tail)));
      return AS_splice(insert_entry,
                       AS_InstrList(AS_Oper("TODO: F_procEntryExit2", NULL,
                                            returnSink, NULL),
                                    NULL));
}

AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
  char buf[80];
  snprintf(buf, 80, "#PROCEDURE %s\n", S_name(frame->frame_label));
  return AS_Proc(String(buf), body, "#END\n");
}

Temp_temp F_FP(void) {
  if (FP == NULL)
    initRegMap();
  return FP;
}

Temp_temp F_RV(void) {
  if (A0 == NULL)
    initRegMap();
  return A0;
}

Temp_temp F_SP(void) {
  if (SP == NULL)
    initRegMap();
  return SP;
}

Temp_tempList F_calldefs(void) {
  // FIXME
  // We either store the RA reg each time when we call other functions or store
  // it at the beginning of each function
  return Temp_TempList(A0, Temp_TempList(RA, callerSaves));
}

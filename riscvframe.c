/* RISC-V frame implementation */

// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc

#include "frame.h"
#include "util.h"

#define ARG_IN_REG 8 // a0-a7
const int F_wordSize = 4;
F_fragList F_fragments = NULL;

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

F_frag F_string(Temp_label lab, string str) {
  F_frag f = F_StringFrag(lab, str);
  F_fragments = F_FragList(f, F_fragments);
  return f;
}

Temp_temp F_FP(void) { return Temp_newtemp(); }

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
  l->tail = f->formals_list;
  f->formals_list = l;
  return ret;
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
  debug("'%s'\n", Temp_labelstring(name));
  F_frame frame = checked_malloc(sizeof(*frame));
  frame->frame_label = name;
  frame->stack_size = 0;
  bool escape;

  if (formals) { // the static links
    escape = formals->head;
    frame->formals_list = NULL;
    F_allocLocal(frame, escape);
    assert(frame->formals_list->head->kind == inFrame);
    debug("%s: installed static links\n", Temp_labelstring(name));
    formals = formals->tail;
  } else { // the global stack
    debug("%s: init global frame\n", Temp_labelstring(name));
    frame->formals_list = NULL;
    return frame;
  }

  while (formals) {
    escape = formals->head;
    F_allocLocal(frame, escape); // TODO how to make sure they are in a0-a7?
    debug("%s: install new param (escape=%d)\n", Temp_labelstring(name),
          escape);
    formals = formals->tail;
  }

  debug("%s\n", Temp_labelstring(name));
  return frame;
}

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

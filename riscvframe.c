/* RISC-V frame implementation */

// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc

#include "frame.h"
#include "util.h"

#define WORD_SIZE 8 // pointer size in bytes in RV64
#define ARG_IN_REG 8   // a0-a7

struct F_access_ {
  enum { inFrame, inReg } kind;
  union {
    int offset;
    Temp_temp reg;

  } u;
};

struct F_frame_ {
  Temp_label frame_label;
  int stack_size;            // the number of locals allocated so far
  F_accessList formals_list; // the locations of all the formals
};

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
          f->stack_size * WORD_SIZE * -1);
    ret = InFrame(f->stack_size * WORD_SIZE * -1);
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
  debug("%s\n", Temp_labelstring(name));
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
    printf("Frame[%d]\n", access->u.offset);
  } else {
    printf("Temp Reg[%d]\n", access->u.reg->num);
  }
}

string F_frameLable(F_frame f) { return Temp_labelstring(f->frame_label); }

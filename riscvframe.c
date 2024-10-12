/* RISC-V frame implementation */

// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc

#include "frame.h"

#define POINTER_SIZE 8 // pointer size in bytes in RV64
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
  assert(0); // not call, we want all escape
  return NULL;
}

F_access F_allocLocal(F_frame f, bool escape) {
  assert(escape);
  return InFrame(f->stack_size * POINTER_SIZE * -1);
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
  F_frame frame = checked_malloc(sizeof(*frame));
  frame->frame_label = name;
  frame->stack_size = 0;
  bool escape = TRUE;
  F_accessList l = NULL;
  if (formals) { // TODO first bool is the static links
    escape = formals->head;
    frame->formals_list = checked_malloc(sizeof(F_accessList));
    l = frame->formals_list;
    l->head = F_allocLocal(frame, escape);
    formals = formals->tail;
  }
  while (formals) {
    escape = formals->head;
    l->tail = checked_malloc(sizeof(*l));
    l = l->tail;
    formals = formals->tail;
  }

  return frame;
}

F_accessList F_formals(F_frame f) {
	return f->formals_list;
}

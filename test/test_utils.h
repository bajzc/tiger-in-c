#ifndef TEST_UTILS_H
#define TEST_UTILS_H
extern "C"{
#include "absyn.h"
#include "escape.h"
#include "frame.h"
#include "parse.h"
#include "semant.h"
}

static inline F_fragList prog2frag(char *path_name) {
  A_exp absyn_root;
  F_fragList frags;
  absyn_root = parse(path_name);
  Esc_findEscape(absyn_root);
  frags = SEM_transProg(absyn_root);
  return frags;
}

#endif //TEST_UTILS_H

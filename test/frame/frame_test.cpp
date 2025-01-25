#include <gtest/gtest.h>
#include "test_utils.h"
extern "C" {
#include "absyn.h"
#include "escape.h"
#include "frame.h"
#include "parse.h"
#include "semant.h"
}

TEST(frame, allocFormals) {
  F_fragList frags = prog2frag("frame/test1.tig");
  EXPECT_EQ(frags->head->kind, F_frag_::F_procFrag);
  EXPECT_STREQ(Temp_labelstring(F_name(frags->head->u.proc.frame)), "_start");
  EXPECT_EQ(frags->tail->head->kind, F_frag_::F_procFrag);
  EXPECT_EQ(frags->tail->tail, nullptr);

  std::vector<F_access> formals;
  for (F_accessList l = F_reverseList(F_formals(frags->tail->head->u.proc.frame)); l;
       l = l->tail) {
    formals.push_back(l->head);
  }
  for (auto i = formals.begin(); i != formals.end(); ++i) {
    if (i - formals.begin() == 0) // static link in reverse order
      EXPECT_EQ(F_isInReg(*i), false);
    else {
      EXPECT_EQ(F_isInReg(*i), true);
    }
  }
}

TEST(frame, allocLocals) {
  F_fragList frags = prog2frag("frame/test1.tig");
  F_frame frame = frags->tail->head->u.proc.frame;
  EXPECT_EQ(F_isInReg(F_allocLocal(frame, true)), false);
  EXPECT_EQ(F_isInReg(F_allocLocal(frame, false)), true);
  EXPECT_EQ(F_isInReg(F_allocLocal(frame, true)), false);
  EXPECT_EQ(F_isInReg(F_allocLocal(frame, true)), false);
  std::vector<F_access> locals;
  for (auto i = F_reverseList(F_locals(frame)); i; i = i->tail) {
    locals.push_back(i->head);
  }
  for (auto i = locals.begin(); i != locals.end(); ++i) {
    if (i - locals.begin() == 1)
      EXPECT_EQ(F_isInReg(*i), true);
    else
      EXPECT_EQ(F_isInReg(*i), false);
  }
}

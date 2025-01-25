#include <gtest/gtest.h>
#include "test_utils.h"
extern "C" {
#include "absyn.h"
#include "env.h"
#include "escape.h"
#include "frame.h"
#include "parse.h"
#include "semant.h"
#include "translate.h"
}

TEST(translate_test, Tr_formals_user_fun_test) {
  S_reset_symbols();
  Temp_resetAll();

  prog2frag("translate/test1.tig");
  E_enventry fun = static_cast<E_enventry>(
      TAB_look(SEM_funLabel2funEntry, Temp_namedlabel("L0")));
  EXPECT_NE(fun, nullptr);
  EXPECT_EQ(fun->kind, E_enventry_::E_funEntry);
  Tr_accessList formals = Tr_formals_with_static_link(fun->u.fun.level);
  EXPECT_EQ(F_isInReg(formals->head->access), false);
  formals = formals->tail;
  EXPECT_EQ(F_isInReg(formals->head->access), true);
  formals = formals->tail;
  EXPECT_EQ(F_isInReg(formals->head->access), true);
  formals = formals->tail;
  EXPECT_EQ(F_isInReg(formals->head->access), true);
  formals = formals->tail;
  EXPECT_EQ(formals, nullptr);
}

TEST(translate_test, Tr_formals_lib_fun_test) {
  S_reset_symbols();
  Temp_resetAll();

  E_base_venv();
  E_enventry fun = static_cast<E_enventry>(TAB_look(E_funLabel2funEntry, Temp_namedlabel("printInt")));
  EXPECT_NE(fun, nullptr);
  fun = static_cast<E_enventry>(TAB_look(E_funLabel2funEntry, Temp_namedlabel("stringEqual")));
  EXPECT_NE(fun, nullptr);
  EXPECT_EQ(fun->kind, E_enventry_::E_funEntry);
  Tr_accessList formals = Tr_formals_with_static_link(fun->u.fun.level);
  EXPECT_EQ(F_isInReg(formals->head->access), true);
  formals = formals->tail;
  EXPECT_EQ(F_isInReg(formals->head->access), true);
  formals = formals->tail;
  EXPECT_EQ(formals, nullptr);
}
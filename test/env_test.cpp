#include <gtest/gtest.h>
extern "C" {
#include "env.h"
#include "symbol.h"
}

TEST(env, base_venv) {
  auto base_venv = E_base_venv();
  auto stringEqual =
      static_cast<E_enventry>(S_look(base_venv, S_Symbol("stringEqual")));
  EXPECT_EQ(stringEqual->kind, E_enventry_::E_funEntry);
  EXPECT_EQ(Temp_labelstring(stringEqual->u.fun.label), "stringEqual");
}

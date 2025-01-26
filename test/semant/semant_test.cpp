#include <gtest/gtest.h>
#include <test_utils.h>

// allow to initialize in different order as declared
TEST(semant, init_record) {
  prog2frag("semant/test1.tig");
}
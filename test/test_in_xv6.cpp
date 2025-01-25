#include <cstdlib>
#include <gtest/gtest.h>

TEST(xv6, queen) {
  EXPECT_EQ(system("./run_xv6.sh queen"), 0);
}

TEST(xv6, merge) {
  EXPECT_EQ(system("./run_xv6.sh merge"), 0);
}
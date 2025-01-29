#include <cstdlib>
#include <gtest/gtest.h>
#include <test_utils.h>

// run the whole compiling process changes some static values, which will break
// later compilation
// So uncomment single test at once
TEST(xv6, merge) {
  // char *argv[2] = {"./tigerc", "../sample/merge.tig"};
  // EXPECT_EQ(pseudo_main(2, argv), 0);
  EXPECT_EQ(system("./run_xv6.sh merge"), 0);
}

TEST(xv6, queen) {
  // char *argv[2] = {"./tigerc", "../sample/queen.tig"};
  // EXPECT_EQ(pseudo_main(2, argv), 0);
  EXPECT_EQ(system("./run_xv6.sh queen"), 0);
}

TEST(xv6, qsort) {
  // char *argv[2] = {"./tigerc", "../sample/qsort.tig"};
  // EXPECT_EQ(pseudo_main(2, argv), 0);
  EXPECT_EQ(system("./run_xv6.sh qsort"), 0);
}

TEST(xv6, libcfun) {
  // char *argv[2] = {"./tigerc", "../sample/libfunc.tig"};
  // EXPECT_EQ(pseudo_main(2, argv), 0);
  EXPECT_EQ(system("./run_xv6.sh libfunc"), 0);
}

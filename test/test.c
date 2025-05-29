#include "sample_lib_test.c"
#include "utils.h"
#include <stdio.h>

int maintest() {
  {
    // Call all test modules you want to run here
    sample_lib_test();
  }
  allsuccess();
}
#include "exec.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int main(int argc, char **argv) {
  return exec_preload(argv[1], argv+1);
}

#pragma GCC diagnostic pop
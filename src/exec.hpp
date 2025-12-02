#ifndef PINCUSHION_EXEC_HPP
#define PINCUSHION_EXEC_HPP

#ifdef WIN32
#elif defined(__APPLE__)
#elif defined(__linux__)
#include <cstdlib>
#include <unistd.h>
#else
#endif

inline int exec_preload(const char *prog_name, char** argv) {
#ifdef WIN32
#elif defined(__APPLE__)
#elif defined(__linux__)
  setenv("LD_PRELOAD", "pincushion_hooks.so", 1);
  return execv(prog_name, argv);
#else
#endif
}

#endif
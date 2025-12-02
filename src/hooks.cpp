#include <cstdint>
#include <iostream>

#include "breakpoint.hpp"
#include "callstack.hpp"
#include "intern.h"
#include "parse_yaml.hpp"

static BreakpointController bc;

static const class initializer {
  public:
  initializer() { 
    hooks_present = 1;
    //bc.add_function_filter("main");
    ConfigParser conf;
    conf.add_breakpoints(bc);
  }
} _initializer;

extern "C" {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// NOLINTNEXTLINE
void __cyg_profile_func_enter(void* this_fn, void* call_site) {
  // take this function off the top of the call stack
  auto cs = boost::stacktrace::stacktrace(1, SIZE_MAX);
  if (bc.should_break(cs)) {
    bc.raise_breakpoint();
  }
}

// NOLINTNEXTLINE
void __cyg_profile_func_exit(void* this_fn, void* call_site) {
}

#pragma GCC diagnostic pop

}  // extern "C"

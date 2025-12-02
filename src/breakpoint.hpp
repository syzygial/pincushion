#ifndef PINCUSHION_BREAKPOINT_HPP
#define PINCUSHION_BREAKPOINT_HPP

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__)
#define __APPLE_API_UNSTABLE
#include <sys/sysctl.h>
#include <unistd.h>

#include <csignal>
#elif defined(__linux__)
#include <cctype>
#include <charconv>
#include <csignal>
#include <fstream>
#include <string_view>
#endif

#include <map>
#include <set>
#include <string>
#include <variant>

#include "callstack.hpp"

class BreakpointFilter {
 public:
  virtual bool match(const boost::stacktrace::stacktrace&) = 0;
  virtual ~BreakpointFilter() = default;
};

class FunctionFilter : public BreakpointFilter {
  public:
  FunctionFilter() = default;
  FunctionFilter(const std::string& fn_name) : fn_name(fn_name) {}
  bool match(const boost::stacktrace::stacktrace& cs) override {
    std::string calling_fn_name = function_from_boost(cs);
    if ((NAME_HAS_FILENAME(calling_fn_name) &&
         NAME_HAS_FILENAME(fn_name)) ||  // both names have filenames
        (!NAME_HAS_FILENAME(calling_fn_name) &&
         !NAME_HAS_FILENAME(fn_name))  // neither name has a filename
    ) {
      return fn_name == calling_fn_name;
    }
    else if (NAME_HAS_FILENAME(calling_fn_name)) {
      return FUNCTION_FROM_NAME(calling_fn_name) == fn_name;
    }
    else if (NAME_HAS_FILENAME(fn_name)) {
      return calling_fn_name == FUNCTION_FROM_NAME(fn_name);
    }
    // else;
    return false;
  }

 private:
  std::string fn_name;
};

// TODO
class CallstackFilter : public BreakpointFilter {};

class BreakpointController {
 public:
  BreakpointController() = default;
  BreakpointController(const BreakpointController&) = delete;
  BreakpointController& operator=(const BreakpointController&) = delete;
  BreakpointController(BreakpointController&&) = delete;
  BreakpointController& operator=(BreakpointController&&) = delete;
  ~BreakpointController() {
    for (auto& brk : breakpoints) {
      delete brk;
    }
  }
  int add_function_filter(const char* function_name) {
    FunctionFilter *new_filter = new FunctionFilter(function_name);
    breakpoints.emplace(new_filter);
    enabled_breakpoints.emplace(new_filter);
    return 0;
  }
  bool should_break(const boost::stacktrace::stacktrace& cs) {
    if (!debugger_present()) {
      return false;
    }
    for (auto& brk_filter : enabled_breakpoints) {
      //auto& brk_filter = breakpoints[brk];
      if (brk_filter->match(cs)) {
        return true;
      }
    }
    return false;
  }

  static bool debugger_present() {
    // debugger_present() and raise_breakpoint() are largely taken from this
    // pull request: https://projects.blender.org/blender/blender/pulls/150555
#ifdef WIN32
    return IsDebuggerPresent();
#elif defined(__APPLE__)
    // similar to the AmIBeingDebugged function in an
    // Apple technical Q&A
    // https://developer.apple.com/library/archive/qa/qa1361/_index.html
    int mib[4] = {0};
    size_t len = 4;
    sysctlnametomib("kern.proc.pid", &mib, &len);
    if (mib != 3) {  // didn't initialize correctly
      return false;
    }
    mib[3] = getpid();
    struct kinfo_proc kp = {0};
    kp.kp_proc.p_flag = 0;
    len = sizeof(kp);
    if (sysctl(mib, 4, &kp, &len, NULL, 0) != 0) {
      return false;
    }
    return (kp.kp_proc.p_flag & P_TRACED) == P_TRACED;
#elif defined(__linux__)
    // check /proc/self/status to see if TracerPid is non-zero,
    // which has been in procfs since linux 2.6.  For more information
    // see https://stackoverflow.com/a/24969863
    std::ifstream proc_status("/proc/self/status");
    std::string line;
    while (!proc_status.eof()) {
      std::getline(proc_status, line);
      std::string_view sv(line.c_str(), 9);  // strlen("TracerPid") = 9
      if (sv != "TracerPid") {
        continue;
      }
      std::string::iterator pid_begin = line.begin();
      while (!std::isdigit(*pid_begin) && pid_begin != line.end()) {
        pid_begin++;
      }
      sv = std::string_view(&(*pid_begin));
      int tracer_pid = 0;
      (void)std::from_chars(sv.data(), sv.data() + sv.length(), tracer_pid);
      return (tracer_pid > 0) ? true : false;
    }
#endif
    // unknown OS
    return false;
  }
  static int raise_breakpoint() {
    if (!debugger_present()) {
      return -1;
    }
    std::cout << "issuing breakpoint" << std::endl;
#ifdef WIN32
    // Wrap DebugBreak in an exception handler so that something going wrong
    // doesn't crash the process
    __try {
      DebugBreak();
      return 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      return -1;
    }
#else  // POSIX-based
    // prefer architecture-specific interrupts since they produce fewer extra
    // stack frames compared to raise(SIGTRAP)
#ifdef __x86_64__
    asm("int3");
#else
    std::raise(SIGTRAP);
#endif

#endif
    return 0;
  }

 private:
  std::set<BreakpointFilter*> breakpoints;
  std::set<BreakpointFilter*> enabled_breakpoints;
};

#endif
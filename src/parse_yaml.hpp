#ifndef PINCUSHION_PARSE_YAML_HPP
#define PINCUSHION_PARSE_YAML_HPP

#ifdef WIN32
#elif defined(__APPLE__)
#elif defined(__linux__)
#include <fstream>
#else
#endif

#include <filesystem>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "breakpoint.hpp"

namespace fs = std::filesystem;

class ConfigParser {
  public:
  ConfigParser() {
    find_config_file();
  }
  ConfigParser(const std::string& filename) {
    this->config = YAML::LoadFile(filename);
  }
  int add_breakpoints(BreakpointController& bc) {
    if (!config.IsDefined()) {
      return -1;
    }
    YAML::Node breakpoint_entries = find_matching_entry();
    for (size_t i = 0; i < breakpoint_entries.size(); i++) {
      auto brk_itr = *(breakpoint_entries[i].begin());
      const std::string brk_type = brk_itr.first.as<std::string>();
      const std::string brk_name = brk_itr.second.as<std::string>();
      if (brk_type == "Function") {
        bc.add_function_filter(brk_name.c_str());
      }
    }
    return 0;
  }
  private:
  int find_config_file() {
    fs::path dir = fs::current_path();
    for(;;) {
      fs::path file_path = dir / ".breakpoints.yaml";
      if (fs::exists(file_path)) {
        config = YAML::LoadFile(file_path);
        return 0;
      }
      dir = dir.parent_path();
      if (dir == dir.root_directory()) {
        return -1;
      }
    }
  }
  std::string get_progname() {
#ifdef WIN32
#elif defined(__APPLE__)
#elif defined(__linux__)
    std::string progname;
    std::ifstream cmdline("/proc/self/cmdline");
    std::getline(cmdline, progname, '\0');
    return progname;
#else
#endif
  }
  YAML::Node find_matching_entry() {
    fs::path progname = fs::path(get_progname()).stem();
    for (const auto& entry : config) {
      std::string entry_name = entry.first.as<std::string>();
      if (entry_name == progname) {
        return entry.second;
      }
    }
    return YAML::Node();
  }
  void validate_config() {}
  YAML::Node config;
};

#endif
#ifndef PINCUSHION_CALLSTACK_HPP
#define PINCUSHION_CALLSTACK_HPP

#include <boost/stacktrace.hpp>
#include <string>
#include <vector>

#include <iostream>

#define NAME_HAS_FILENAME(str) ((str).find_first_of(':') < (str).size())
#define FILENAME_FROM_NAME(str) ((NAME_HAS_FILENAME(str)) ? (str).substr(0, (str).find_first_of(':')) : "")
#define FUNCTION_FROM_NAME(str) ((NAME_HAS_FILENAME(str)) ? (str).substr((str).find_first_of(':')+1) : (str))

using CallStack = std::vector<std::string>;

inline std::string str_from_boost_frame(const boost::stacktrace::frame& frame) {
  std::string frame_str = boost::stacktrace::to_string(frame);
  std::string combined_name;
  size_t function_name_end = frame_str.find_first_of(' ');
  if (frame_str.find(" at ") < frame_str.size()) {  // filename found
    std::string function_name = frame_str.substr(0, function_name_end);
    size_t file_name_start =
        frame_str.find_first_of(' ', function_name_end + 1) + 1;
    size_t file_name_end = frame_str.find_first_of(':', file_name_start);
    std::string file_name =
        frame_str.substr(file_name_start, file_name_end - file_name_start);
    combined_name = file_name + ':' + function_name;
  }
  else {
    combined_name = frame_str.substr(0, function_name_end);
  }
  return combined_name;
}

inline std::string function_from_boost(
    const boost::stacktrace::stacktrace& cs) {
  auto& top_frame = cs[0];
  return str_from_boost_frame(top_frame);
}

inline CallStack callstack_from_boost(const boost::stacktrace::stacktrace& cs) {
  CallStack new_cs;
  for (auto& frame : cs) {
    new_cs.push_back(str_from_boost_frame(frame));
  }
  return new_cs;
}

#endif

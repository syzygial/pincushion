include(FindPackageHandleStandardArgs)
find_path(YamlCpp_INCLUDE_DIR NAMES yaml-cpp/yaml.h)
find_library(YamlCpp_LIBRARY NAMES yaml-cpp)
find_package_handle_standard_args(YamlCpp DEFAULT_MSG
  YamlCpp_LIBRARY YamlCpp_INCLUDE_DIR)
if (YamlCpp_FOUND)
  set(YamlCpp_LIBRARIES ${YamlCpp_LIBRARY})
  set(YamlCpp_INCLUDE_DIRS ${YamlCpp_INCLUDE_DIR})
endif()
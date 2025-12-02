cmake_minimum_required(VERSION 3.10)

project(pincushion VERSION 0.1.0)

cmake_policy(SET CMP0167 NEW)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})

# Courtesy of https://stackoverflow.com/a/50882216
# CC-BY-SA 4.0
if(MSVC)
  add_compile_options(/W4)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/WX)
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Werror)
  endif()
endif()

find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)
find_package(YamlCpp REQUIRED)

add_executable(pincushion src/pincushion.cpp)

add_library(libpincushion SHARED)
set_target_properties(libpincushion PROPERTIES PREFIX "")
target_include_directories(libpincushion PRIVATE src/include)
target_sources(libpincushion PRIVATE src/lib.cpp)

add_library(pincushion_hooks SHARED)
set_target_properties(pincushion_hooks PROPERTIES PREFIX "")
target_link_libraries(pincushion_hooks PUBLIC libpincushion)
target_sources(pincushion_hooks PRIVATE src/hooks.cpp)
target_link_libraries(pincushion_hooks PUBLIC Boost::stacktrace_backtrace)
target_link_libraries(pincushion_hooks PUBLIC yaml-cpp)
target_compile_definitions(pincushion_hooks PUBLIC BOOST_STACKTRACE_USE_BACKTRACE=1)
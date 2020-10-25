cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.12)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.0.1 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)

find_cppkg(fmt 7.0.3  MODULE fmt::fmt TYPE lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
find_cppkg(nlohmann_json 3.9.1  MODULE nlohmann_json::nlohmann_json TYPE lib)
find_cppkg(tomlpp git  MODULE tomlpp::tomlpp TYPE lib)
endif()


cppm_target_define(serdepp INTERFACE)


set(global_deps PUBLIC fmt)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   list(APPEND global_deps PUBLIC nlohmann_json tomlpp)
endif()
cppm_target_dependencies(serdepp
   ${global_deps})


cppm_target_install(serdepp)


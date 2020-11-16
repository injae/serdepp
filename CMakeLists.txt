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
find_cppkg(toml11 3.6.0  MODULE toml11::toml11 TYPE lib)
find_cppkg(tomlpp git  MODULE tomlpp::tomlpp TYPE lib)
find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib)
endif()


cppm_target_define(serdepp INTERFACE)

cppm_target_define(nlohmann_json_example BINARY
SOURCES
    examples/nlohmann_json_example.cpp
)

cppm_target_define(tomlpp_example BINARY
SOURCES
    examples/tomlpp_example.cpp
)

cppm_target_define(toml11_example BINARY
SOURCES
    examples/toml11_example.cpp
)


set(global_deps PUBLIC fmt)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   list(APPEND global_deps PUBLIC nlohmann_json toml11 tomlpp yaml-cpp)
endif()
cppm_target_dependencies(serdepp
   ${global_deps})

cppm_target_dependencies(nlohmann_json_example
   ${global_deps}
   serdepp)

cppm_target_dependencies(tomlpp_example
   ${global_deps}
   serdepp)

cppm_target_dependencies(toml11_example
   ${global_deps}
   serdepp)


cppm_target_install(serdepp)
cppm_target_install(nlohmann_json_example)
cppm_target_install(tomlpp_example)
cppm_target_install(toml11_example)


cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.13)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.0.1 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)

find_cppkg(fmt 7.1.3  MODULE fmt::fmt TYPE lib)
find_cppkg(nlohmann_json 3.9.1  MODULE nlohmann_json::nlohmann_json TYPE lib OPTIONAL OFF)
find_cppkg(toml11 3.6.0  MODULE toml11::toml11 TYPE lib OPTIONAL OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib OPTIONAL OFF)
endif()


cppm_target_define(serdepp INTERFACE)

cppm_examples_area()
if(SERDEPP_BUILD_EXAMPLES)

cppm_target_define(nlohmann_json_example BINARY
SOURCES
    examples/nlohmann_json_example.cpp
)

cppm_target_define(toml11_example BINARY
SOURCES
    examples/toml11_example.cpp
)

cppm_target_define(convert_example BINARY
SOURCES
    examples/convert_example.cpp
)

cppm_target_define(test_all BINARY
SOURCES
    examples/variant_test.cpp
)

cppm_target_define(overwrite BINARY
SOURCES
    examples/overwrite.cpp
)

endif()


set(serdepp_global_deps PUBLIC fmt nlohmann_json toml11)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   list(APPEND serdepp_global_deps PUBLIC yaml-cpp)
endif()
cppm_target_dependencies(serdepp
   ${serdepp_global_deps})

cppm_target_dependencies(nlohmann_json_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(toml11_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(convert_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(test_all
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(overwrite
   ${serdepp_global_deps}
   serdepp)


cppm_target_install(serdepp)


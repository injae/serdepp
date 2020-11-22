cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.12)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.0.1 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)

find_cppkg(fmt 7.0.3  MODULE fmt::fmt TYPE lib)
find_cppkg(nlohmann_json 3.9.1  MODULE nlohmann_json::nlohmann_json TYPE lib)
find_cppkg(toml11 3.6.0  MODULE toml11::toml11 TYPE lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
find_cppkg(tomlpp git  MODULE tomlpp::tomlpp TYPE lib)
find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib)
endif()


cppm_target_define(serdepp STATIC
SOURCES
    src/to_static.cpp
)

cppm_target_define(test_all BINARY
SOURCES
    examples/variant_test.cpp
)

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

endif()


set(global_deps PUBLIC fmt nlohmann_json toml11)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   list(APPEND global_deps PUBLIC tomlpp yaml-cpp)
endif()
cppm_target_dependencies(serdepp
   ${global_deps})

cppm_target_dependencies(test_all
   ${global_deps}
   serdepp)

cppm_target_dependencies(nlohmann_json_example
   ${global_deps}
   serdepp)

cppm_target_dependencies(toml11_example
   ${global_deps}
   serdepp)

cppm_target_dependencies(convert_example
   ${global_deps}
   serdepp)


cppm_target_install(serdepp)
cppm_target_install(test_all)


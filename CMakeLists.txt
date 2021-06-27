cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.13)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.1.0 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)

find_cppkg(benchmark 1.5.2  MODULE benchmark::benchmark TYPE lib OPTIONAL OFF)
find_cppkg(fmt 7.1.3  MODULE fmt::fmt TYPE lib)
find_cppkg(magic_enum 0.7.3  MODULE magic_enum::magic_enum TYPE lib)
find_cppkg(nameof 0.10.0  MODULE nameof::nameof TYPE lib)
find_cppkg(nlohmann_json 3.9.1  MODULE nlohmann_json::nlohmann_json TYPE lib OPTIONAL OFF)
find_cppkg(toml11 3.7.0  MODULE toml11::toml11 TYPE lib OPTIONAL OFF)
find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib OPTIONAL OFF)


cppm_target_define(serdepp STATIC
SOURCES
    src/to_static.cpp
)

cppm_examples_area()
if(SERDEPP_BUILD_EXAMPLES)

cppm_target_define(serde_example BINARY
SOURCES
    examples/example.cpp
)

cppm_target_define(simple_example BINARY
SOURCES
    examples/simple_example.cpp
)

endif()


set(serdepp_global_deps PUBLIC benchmark fmt magic_enum nameof nlohmann_json toml11 yaml-cpp)
cppm_target_dependencies(serdepp
   ${serdepp_global_deps})

cppm_target_dependencies(serde_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(simple_example
   ${serdepp_global_deps}
   serdepp)


cppm_target_install(serdepp)


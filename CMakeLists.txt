cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.13)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.0.2 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)

find_cppkg(benchmark 1.5.2  MODULE benchmark::benchmark TYPE lib)
find_cppkg(fmt 7.1.3  MODULE fmt::fmt TYPE lib)
find_cppkg(magic_enum 0.7.3  MODULE magic_enum::magic_enum TYPE lib)
find_cppkg(nameof 0.10.0  MODULE nameof::nameof TYPE lib)
find_cppkg(nlohmann_json 3.9.1  MODULE nlohmann_json::nlohmann_json TYPE lib)
find_cppkg(toml11 3.6.0  MODULE toml11::toml11 TYPE lib)
find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib)


cppm_target_define(serdepp STATIC
SOURCES
    src/to_static.cpp
)

cppm_target_define(serde_test2 BINARY
SOURCES
    test/test2.cpp
)

cppm_target_define(benchmark2 BINARY
SOURCES
    benchmark/benchmark_v2_2.cpp
)


set(serdepp_global_deps PUBLIC benchmark fmt magic_enum nameof nlohmann_json toml11 yaml-cpp)
cppm_target_dependencies(serdepp
   ${serdepp_global_deps})

cppm_target_dependencies(serde_test2
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(benchmark2
   ${serdepp_global_deps}
   serdepp)


cppm_target_install(serdepp)
cppm_target_install(serde_test2)
cppm_target_install(benchmark2)


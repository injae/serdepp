cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.13)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.1.4 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(
   DEBUG
      CLANG "-fprofile-instr-generate -fcoverage-mapping"
      GCC   "--coverage -fno-inline -fno-inline-small-functions -fno-default-inline"
)

find_cppkg(benchmark 1.5.2  MODULE benchmark::benchmark TYPE lib OPTIONAL OFF)
find_cppkg(RapidJSON 1.1.1  MODULE rapidjson TYPE lib OPTIONAL OFF)
find_cppkg(nlohmann_json 3.10.5  MODULE nlohmann_json::nlohmann_json TYPE lib OPTIONAL OFF)
find_cppkg(Catch2 3.5.0  MODULE Catch2::Catch2WithMain TYPE lib OPTIONAL OFF)
find_cppkg(toml11 3.7.0  MODULE toml11::toml11 TYPE lib OPTIONAL OFF)
find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib OPTIONAL OFF)
find_cppkg(fmt 8.0.1  MODULE fmt::fmt-header-only TYPE lib OPTIONAL OFF)
find_cppkg(magic_enum 0.7.3  MODULE magic_enum::magic_enum TYPE lib)
find_cppkg(nameof 0.10.0  MODULE nameof::nameof TYPE lib)


cppm_target_define(serdepp STATIC
SOURCES
    src/to_static.cpp
)

cppm_examples_area()
if(SERDEPP_BUILD_EXAMPLES)

cppm_target_define(serde_example1 BINARY
SOURCES
    examples/example1.cpp
)

cppm_target_define(serde_example BINARY
SOURCES
    examples/example.cpp
)

cppm_target_define(simple_example BINARY
SOURCES
    examples/simple_example.cpp
)

cppm_target_define(flatten_example BINARY
SOURCES
    examples/flatten.cpp
)

cppm_target_define(variant_example BINARY
SOURCES
    examples/variant.cpp
)

cppm_target_define(parse_file_example BINARY
SOURCES
    examples/parse_file.cpp
)

cppm_target_define(struct_attribute_example BINARY
SOURCES
    examples/struct_attribute.cpp
)

cppm_target_define(sugar BINARY
SOURCES
    examples/sugar.cpp
)

cppm_target_define(reflection BINARY
SOURCES
    examples/reflection.cpp
)

cppm_target_define(print BINARY
SOURCES
    examples/print.cpp
)

cppm_target_define(pointer BINARY
SOURCES
    examples/pointer.cpp
)

endif()

cppm_unit_test_area()
if(SERDEPP_BUILD_TESTING)

cppm_target_define(unittest BINARY
SOURCES
    tests/serializer.cpp
    tests/rapid_json.cpp
    tests/yaml_cpp.cpp
    tests/toml11.cpp
    tests/reflection.cpp
    tests/nlohmann_json.cpp
    tests/test_struct.hpp
)

endif()
end_cppm_unit_test_area()

if(SERDEPP_BUILD_BENCHMARKS)

cppm_target_define(benchmark BINARY
SOURCES
    benchmark/benchmark.cpp
)

cppm_target_define(data_type_benchmark BINARY
SOURCES
    benchmark/data_type_benchmark.cpp
)

cppm_target_define(syntax_sugar_benchmark BINARY
SOURCES
    benchmark/syntax_benchmark.cpp
)

endif()

set(serdepp_global_deps PRIVATE benchmark Catch2
PUBLIC RapidJSON nlohmann_json toml11 yaml-cpp fmt magic_enum nameof)
cppm_target_dependencies(serdepp
   ${serdepp_global_deps})

cppm_target_dependencies(serde_example1
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(serde_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(simple_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(flatten_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(variant_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(parse_file_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(struct_attribute_example
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(sugar
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(reflection
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(print
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(pointer
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(unittest
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(benchmark
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(data_type_benchmark
   ${serdepp_global_deps}
   serdepp)

cppm_target_dependencies(syntax_sugar_benchmark
   ${serdepp_global_deps}
   serdepp)


cppm_target_install(serdepp)
cppm_target_install(unittest)
cppm_target_install(serde_example1)
cppm_target_install(serde_example)
cppm_target_install(simple_example)
cppm_target_install(flatten_example)
cppm_target_install(variant_example)
cppm_target_install(parse_file_example)
cppm_target_install(struct_attribute_example)
cppm_target_install(sugar)
cppm_target_install(reflection)
cppm_target_install(print)
cppm_target_install(pointer)


cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.13)
include(cmake/cppm_loader.cmake)
cppm_project()

project(serdepp VERSION 0.1.4 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(
   DEBUG
      CLANG "-fprofile-instr-generate -fcoverage-mapping --coverage"
)

find_cppkg(Catch2 3.5.0  MODULE Catch2::Catch2 TYPE lib OPTIONAL OFF)
find_cppkg(RapidJSON 1.1.1  MODULE rapidjson TYPE lib OPTIONAL OFF)
find_cppkg(benchmark 1.5.2  MODULE benchmark::benchmark TYPE lib OPTIONAL OFF)
find_cppkg(fmt 10.2.0  MODULE fmt::fmt-header-only TYPE lib OPTIONAL OFF)
find_cppkg(magic_enum 0.9.5  MODULE magic_enum::magic_enum TYPE lib)
find_cppkg(nameof 0.10.3  MODULE nameof::nameof TYPE lib)
find_cppkg(nlohmann_json 3.11.3  MODULE nlohmann_json::nlohmann_json TYPE lib OPTIONAL OFF)
find_cppkg(toml11 3.7.1  MODULE toml11::toml11 TYPE lib OPTIONAL OFF)
find_cppkg(yaml-cpp 0.8.0  MODULE yaml-cpp TYPE lib OPTIONAL OFF)


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


set(serdepp_global_deps PUBLIC Catch2 RapidJSON benchmark fmt magic_enum nameof nlohmann_json toml11 yaml-cpp)
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


cppm_target_install(serdepp)


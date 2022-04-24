#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/toml11.hpp>

using namespace serde;

using namespace toml::literals;

TEST_CASE("5: toml11 struct (pass)", "[multi-file:5]") {
    toml::value toml_vl = R"(
    str = "hello"
    i = 10
    vec = [ "one", "two", "three" ]
    opt = "hello"
    [sm]
    one = "tone"
    two = "ttwo"
    [sub]
    str = "hello"
    )"_toml;

    REQUIRE(toml_vl == serialize<toml::value>(deserialize<test>(toml_vl)));
}

TEST_CASE("5: toml11 struct from_file (pass)", "[multi-file:5]") {
    auto toml_vl = serde::parse_file<toml::value>("../tests/test.toml");
    REQUIRE(toml_vl == serialize<toml::value>(deserialize<test>(toml_vl)));
}

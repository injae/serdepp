#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/toml11.hpp>

using namespace serde;

using namespace toml::literals;

TEST_CASE("4: toml11 struct (pass)", "[multi-file:4]") {
    auto toml_vl = serde::parse_file<toml::value>("../../tests/test.toml");
    REQUIRE(toml_vl == serialize<toml::value>(deserialize<test>(toml_vl)));
}

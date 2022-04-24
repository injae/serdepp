#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/toml11.hpp>

using namespace serde;

using namespace toml::literals;

TEST_CASE("5: toml11 struct (pass)", "[multi-file:5]") {
    auto toml_vl = serde::parse_file<toml::value>("../tests/test.toml");
    std::cout << serialize<toml::value>(deserialize<test>(toml_vl)) << "\n";
    std::cout << toml_vl << "\n";
    REQUIRE(toml_vl == serialize<toml::value>(deserialize<test>(toml_vl)));
}

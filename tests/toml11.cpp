#include "test_struct.hpp"
#include <catch2/catch_all.hpp>
#include <serdepp/adaptor/toml11.hpp>

using namespace serde;

using namespace toml::literals;

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

TEST_CASE("4: toml11 struct (pass)", "[multi-file:4]") {
  REQUIRE(toml_vl == serialize<toml::value>(deserialize<test>(toml_vl)));
}

#include "test_struct.hpp"
#include <catch2/catch_all.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

nlohmann::json json_v = R"({
"str" : "hello",
"i": 10,
"vec": [ "one", "two", "three"],
"sm": { "one" : "tone", "two" : "ttwo"},
"opt": "hello",
"sub" : { "str": "hello" }
})"_json;

using namespace serde;

TEST_CASE("2: nlohmann json struct (pass)", "[multi-file:2]") {
  REQUIRE(json_v.dump() ==
          serialize<nlohmann::json>(deserialize<test>(json_v)).dump());
}

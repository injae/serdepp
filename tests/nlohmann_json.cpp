
#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/nlohmann_json.hpp>


using namespace serde;

TEST_CASE("2: nlohmann json struct (pass)", "[multi-file:2]") {
    auto json_v = serde::parse_file<nlohmann::json>("../tests/test.json");
    REQUIRE(json_v.dump() == serialize<nlohmann::json>(deserialize<test>(json_v)).dump());
}

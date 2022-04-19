#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/yaml-cpp.hpp>

using namespace serde;

TEST_CASE("5: yaml-cpp struct (pass)", "[multi-file:5]") {
    std::ostringstream convert;
    auto yaml_v = serde::parse_file<YAML::Node>("../tests/test.yaml");
    std::ostringstream origin;
    origin << yaml_v;
    convert << serialize<YAML::Node>(deserialize<test>(yaml_v));

    REQUIRE(origin.str() == convert.str());
}

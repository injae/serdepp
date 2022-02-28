#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/yaml-cpp.hpp>


YAML::Node yaml_v = YAML::Load(R"(
str: hello
i: 10
vec:
- one
- two
- three 
opt: hello
sm:
  one: tone
  two: ttwo
sub:
  str: hello
)");

using namespace serde;

TEST_CASE("5: yaml-cpp struct (pass)", "[multi-file:5]") {
    std::ostringstream origin;
    origin << yaml_v;
    std::ostringstream convert;
    convert << serialize<YAML::Node>(deserialize<test>(yaml_v));

    REQUIRE(origin.str() == convert.str());
}

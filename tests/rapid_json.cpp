#include "test_struct.hpp"
#include <catch2/catch_all.hpp>
#include <serdepp/adaptor/rapidjson.hpp>

std::string str(rapidjson::Document &doc) {
  using namespace rapidjson;
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
};

using namespace serde;

TEST_CASE("5: toml11 struct (pass)", "[multi-file:5]") {
  rapidjson::Document json_v;
  json_v.Parse(R"({
"str" : "hello",
"i": 10,
"vec": [ "one", "two", "three"],
"opt": "hello",
"sm": { "one" : "tone", "two" : "ttwo"},
"sub" : { "str": "hello" }
})");
  rapidjson::Document json_c =
      serialize<rapidjson::Document>(deserialize<test>(json_v));

  REQUIRE(str(json_v) == str(json_c));
}

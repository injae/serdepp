#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/rapidjson.hpp>

std::string str(rapidjson::Document& doc) {
    using namespace rapidjson;
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
};

using namespace serde;


TEST_CASE("3: rapidjson struct (pass)", "[multi-file:3]") {
    auto json_v = serde::parse_file<rapidjson::Document>("../tests/test.json");
    rapidjson::Document json_c = serialize<rapidjson::Document>(deserialize<test>(json_v));
    REQUIRE(str(json_v) == str(json_c));
}

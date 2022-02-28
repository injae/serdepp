#include <catch2/catch.hpp>
#include "test_struct.hpp"
#include <serdepp/adaptor/reflection.hpp>

using namespace serde;

TEST_CASE("3: reflection struct (pass)", "[multi-file:3]") {
    REQUIRE(std::is_same_v<to_tuple_t<test>,
            std::tuple<std::string,
            int,
            std::vector<std::string>,
            std::optional<std::string>,
            std::optional<std::string>,
            std::map<std::string, std::string>,
            nested>>);
}



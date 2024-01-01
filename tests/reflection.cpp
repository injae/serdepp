#include "test_struct.hpp"
#include <catch2/catch_all.hpp>
#include <serdepp/adaptor/reflection.hpp>

using namespace serde;

TEST_CASE("3: reflection struct (pass)", "[multi-file:3]") {
  REQUIRE(std::is_same_v<
          to_tuple_t<test>,
          std::tuple<std::string, int, std::vector<std::string>,
                     std::optional<std::string>, std::optional<std::string>,
                     std::map<std::string, std::string>, nested>>);
  REQUIRE(serde::type_info<test>.size == 7);
  REQUIRE(serde::tuple_size_v<test> == 7);
  REQUIRE(std::is_same_v<std::string,
                         std::tuple_element_t<0, serde::to_tuple_t<test>>>);
}

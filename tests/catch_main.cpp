#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

TEST_CASE("1: test (pass)", "[multi-file:1]") {
    REQUIRE(1==1);
}

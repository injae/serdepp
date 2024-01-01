#include "serdepp/exception.hpp"
#include "serdepp/serializer.hpp"
#include "test_struct.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

struct unknown_adapter {};

TEST_CASE("5: unknown adapter with exception (pass)", "[multi-file:5]") {
    auto adapter = unknown_adapter{};
    using helper_t = serde::derive_serde_adaptor_helper<unknown_adapter>;
    REQUIRE_THROWS_AS(helper_t::is_null(adapter, "hello"), serde::unimplemented_error);
    REQUIRE_THROWS_AS(helper_t::size(adapter), serde::unimplemented_error);
    REQUIRE_THROWS_AS(helper_t::is_struct(adapter), serde::unimplemented_error);
    REQUIRE_THROWS_AS(helper_t::parse_file(std::string("path")), serde::unimplemented_error);
}


#include <serdepp/serde.hpp>

#pragma once

#ifndef __CATCH_TEST_STRUCT_HPP__
#define __CATCH_TEST_STRUCT_HPP__

struct nested {
    DERIVE_SERDE(nested, _SF_(str))
    std::string str;
};

struct Rect {
    DERIVE_SERDE(Rect, (&Self::width, "width")(&Self::height, "height"))
    int width;
    int height;
};

struct Circle {
    DERIVE_SERDE(Circle, (&Self::radius, "radius"))
    int radius;
};


struct test {
    DERIVE_SERDE(test, _SF_(str)_SF_(i)_SF_(vec)_SF_(opt)_SF_(none_opt)_SF_(sm)_SF_(sub))
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::optional<std::string> none_opt;
    std::optional<std::string> opt;
    std::map<std::string, std::string> sm;
    nested sub;
};

struct variant_test {
    DERIVE_SERDE(variant_test, _SF_(opt_var)_SF_(var))
    std::variant<std::monostate, Circle, Rect> opt_var;
    std::variant<Circle, Rect> var;
};


#endif


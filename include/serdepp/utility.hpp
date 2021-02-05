#pragma once

#include "serdepp/serializer.hpp"
#include "serdepp/adaptor/fmt.hpp"
#include <cstdarg>

#ifndef __SERDE_UTILITY_HPP__
#define __SERDE_UTILITY_HPP__


#define SERDE_TAG_1_ARGS(var) tag(var, #var)
#define SERDE_TAG_2_ARGS(var, default_var) tag(var, #var, default_var)
#define SERDE_GET_3TH_ARGS(arg1, arg2, arg3, ...) arg3
#define SERDE_TAG_MACRO_CHOOSER(...) SERDE_GET_3TH_ARGS(__VA_ARGS__, SERDE_TAG_2_ARGS, SERDE_TAG_1_ARGS, )
#define TAG(...) SERDE_TAG_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define derive_serde(TYPE, ...) friend serde::serializer<TYPE>; \
    template<typename Ctx> auto serde(Ctx& ctx) { \
        __VA_ARGS__ \
    } \
    friend std::ostream& operator<<(std::ostream& os, const TYPE& type) { \
    return os << serde::deserialize_with_name<serde::literal>(type, #TYPE).to_string(); \
    }                                           

//#define TAG(VAR) tag(VAR, #VAR)
#define TAG_OR(VAR, DEFAULT_VALUE) tag(VAR, #VAR, DEFAULT_VALUE)  // DUPLICATED MACRO

#endif

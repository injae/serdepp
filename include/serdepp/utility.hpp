#pragma once

#include "serdepp/serializer.hpp"
#include "serdepp/attributes.hpp"
#include <cstdarg>

#ifndef __SERDE_UTILITY_HPP__
#define __SERDE_UTILITY_HPP__


//#define SERDE_TAG_1_ARGS(var) tag(var, #var)
//#define SERDE_TAG_2_ARGS(var, default_var) tag(var, #var, default_var)
//#define SERDE_GET_3TH_ARGS(arg1, arg2, arg3, ...) arg3
//#define SERDE_TAG_MACRO_CHOOSER(...) SERDE_GET_3TH_ARGS(__VA_ARGS__, SERDE_TAG_2_ARGS, SERDE_TAG_1_ARGS, )
//#define TAG(...) SERDE_TAG_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define DERIVE_SERDE(Type, ...) \
    template<class Context> \
    constexpr static void serde(Context& context, Type& value) {\
        using namespace serde::attribute;    \
        using Self [[maybe_unused]] = Type; \
        serde::serde_struct ss(context, value); \
        ss \
            __VA_ARGS__;                     \
    }                                        \

#define _SF_(name) (&Self::name, #name)

#endif

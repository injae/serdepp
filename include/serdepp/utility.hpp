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
    constexpr static auto serde(Context& context, Type& value) {\
        using namespace serde::attribute;    \
        using Self [[maybe_unused]] = Type; \
        serde::serde_struct ss(context, value); \
        return ss \
            __VA_ARGS__;                     \
    }                                        \

#define DERIVE_SERDE_NON_INTRUSIVE(Type, ...) \
namespace serde { \
    using namespace attribute; \
    template<typename serde_ctx> \
    struct serde_serializer<Type, serde_ctx> { \
        using Adaptor = typename serde_ctx::Adaptor; \
        constexpr inline static auto from(serde_ctx& ctx, Type& data, std::string_view key) { \
            serde_struct ss(ctx, data); \
            using Self [[maybe_unused]] = Type; \
            ss \
                __VA_ARGS__; \
            ctx.read(); \
        }\
\
        constexpr inline static auto into(serde_ctx& ctx, const Type& data, std::string_view key) { \
            auto struct_ctx = serde_context<Adaptor, true>(ctx.adaptor); \
            serde_struct ss(struct_ctx, const_cast<Type&>(data)); \
            using Self [[maybe_unused]] = Type; \
            ss \
                __VA_ARGS__; \
            ctx.read(); \
        } \
    }; \
    namespace meta { \
        template<class Context> struct is_serdeable<Context, Type, void> : std::true_type {}; \
    } \
}

#define _SF_(name) (&Self::name, #name)

#endif

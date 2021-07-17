#pragma once

#ifndef __SERDEPP_ATTRIABUTE_DEFAULT_HPP__
#define __SERDEPP_ATTRIABUTE_DEFAULT_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    template<typename D>
    struct default_se {
        D&& default_value_;
        [[deprecated("changed -> default_")]]
        explicit default_se(D&& default_value) noexcept : default_value_(std::move(default_value)) {}
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_null(ctx.adaptor, key)) {
                data = std::move(default_value_);
            } else {
                next_attr.from(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.into(ctx, data, key, remains...);
        }
    };
    // deduce guide
    template<typename D> default_se(D&&) -> default_se<D>;

    template<typename D>
    struct default_ {
        D&& default_value_;
        explicit default_(D&& default_value) noexcept : default_value_(std::move(default_value)) {}
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_null(ctx.adaptor, key)) {
                data = std::move(default_value_);
            } else {
                next_attr.from(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.into(ctx, data, key, remains...);
        }
    };
    // deduce guide
    template<typename D> default_(D&&) -> default_<D>;
}


#endif

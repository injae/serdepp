#pragma once

#ifndef __SERDEPP_ATTRIBUTE_HPP__
#define __SERDEPP_ATTRIBUTE_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    template<typename D>
    struct set_default {
        D&& default_value_;
        explicit set_default(D&& default_value) noexcept : default_value_(std::move(default_value)) {}
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_null(ctx.adaptor, key)) {
                data = std::move(default_value_);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };

    template<typename D> set_default(D&&) -> set_default<D>;

    struct value_or_struct {
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_struct(ctx.adaptor)) {
                next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
            } else {
                next_attr.template from<T, serde_ctx>(ctx, data, "", remains...);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };
}


#endif

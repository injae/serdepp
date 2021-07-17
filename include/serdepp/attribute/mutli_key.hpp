#pragma once

#ifndef __SERDEPP_ATTRIBUTE_MULTI_KEY_HPP__
#define __SERDEPP_ATTRIBUTE_MULTI_KEY_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    struct multi_key {
        std::vector<std::string_view> keys_;
        explicit multi_key(std::initializer_list<std::string_view> keys) : keys_(keys) {}

        template<typename serde_ctx>
        inline std::string_view find_key(serde_ctx& ctx, std::string_view key) const{
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            for(auto multi_key : keys_) { if(!Helper::is_null(ctx.adaptor, multi_key)) return multi_key; }
            return key;
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void from(serde_ctx& ctx, T& data, std::string_view key,
                        Next&& next_attr, Attributes&&... remains) const {
            next_attr.from(ctx, data, find_key(ctx, key), remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                        Next&& next_attr, Attributes&&... remains) const {
            next_attr.into(ctx, data, key, remains...);
        }
    };
}


#endif

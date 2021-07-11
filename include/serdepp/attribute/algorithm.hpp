#pragma once

#ifndef __SERDEPP_ATTRIBUTE_ALGORITHM_HPP__
#define __SERDEPP_ATTRIBUTE_ALGORITHM_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    template<typename FUNC>
    struct transform {
        transform(FUNC&& serialize, FUNC&& deserialize)
            : serialize_(std::move(serialize)), deserialize_(std::move(deserialize)){}

        template<typename T, typename serde_ctx>
        inline void from(serde_ctx& ctx, T& data, std::string_view key) const {
            std::transform(data.begin(), data.end(), data.begin(), serialize_);
        }

        template<typename T, typename serde_ctx>
        inline void into(serde_ctx& ctx, T& data, std::string_view key) const {
            std::transform(data.begin(), data.end(), data.begin(), deserialize_);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void from(serde_ctx& ctx, T& data, std::string_view key,
                            Next&& next_attr, Attributes&&... remains) const {
            std::transform(data.begin(), data.end(), data.begin(), serialize_);
            next_attr.from(ctx, data, key, std::forward<Attributes>(remains)...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                            Next&& next_attr, Attributes&&... remains) const{
            std::transform(data.begin(), data.end(), data.begin(), deserialize_);
            next_attr.into(ctx, data, key, std::forward<Attributes>(remains)...);
        }

        FUNC&& serialize_;
        FUNC&& deserialize_;
    };
}


#endif

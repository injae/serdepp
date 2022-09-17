#pragma once

#ifndef __SERDEPP_ATTRIBUTE_UTILITY_HPP__
#define __SERDEPP_ATTRIBUTE_UTILITY_HPP__

#define SERDEPP_DEFAULT_ATTRIBUTE_FROM() \
    template<typename T, typename serde_ctx, typename Next, typename ...Attributes> \
    constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key, Next&& next_attr, Attributes&&... remains) { \
        next_attr.from(ctx, data, key, remains...); \
    } 
    

#define SERDEPP_DEFAULT_ATTRIBUTE_INTO() \
    template<typename T, typename serde_ctx, typename Next, typename ...Attributes> \
    inline void into(serde_ctx& ctx, T& data, std::string_view key, Next&& next_attr, Attributes&&... remains) { \
        next_attr.into(ctx, data, key, remains...); \
    }

#endif

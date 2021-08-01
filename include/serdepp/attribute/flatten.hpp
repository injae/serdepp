#pragma once

#ifndef __SERDEPP_FLATTEN_HPP__
#define __SERDEPP_FLATTEN_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    namespace detail {
        struct flatten {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                       Next&& next_attr, Attributes&& ...remains) const{
                using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
                if(Helper::is_null(ctx.adaptor,key)) {
                    next_attr.from(ctx, data, "", remains...);
                }
                else {
                    next_attr.from(ctx, data, key, remains...);
                }
            }

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                       Next&& next_attr, Attributes&& ...remains) const{
                next_attr.into(ctx, data, key, remains...);
            }
        };
    }
    
    inline constexpr static detail::flatten flatten = detail::flatten{};

}



#endif

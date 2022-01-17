#pragma once

#ifndef __SERDEPP_ATTRIBUTE_MAKE_OPTIONAL_HPP__
#define __SERDEPP_ATTRIBUTE_MAKE_OPTIONAL_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    namespace detail {
        struct make_optional {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                       Next&& next_attr, Attributes&& ...remains) const{
                using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
                if constexpr (meta::is_emptyable_v<T>) {
                    if (Helper::is_null(ctx.adaptor, key)) {
                        if constexpr(std::is_same_v<T, bool>) {

                        } else {
                            if (data.empty()) data = std::move(T{});
                        }
                    } else {
                        next_attr.from(ctx, data, key, remains...);
                    }
                } else {
                    throw serde::attribute_error("this "
                                                 + std::string(nameof::nameof_short_type<T>())
                                                 + " is not emptyable type");
                }
            }

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                       Next&& next_attr, Attributes&& ...remains) const{
                if constexpr (std::is_same_v<T, bool>) {
                    if(data == true)   { next_attr.into(ctx, data, key, remains...); }
                } else {
                    if (!data.empty()) { next_attr.into(ctx, data, key, remains...); }
                }
            }
        };
    }
    
    inline constexpr static detail::make_optional make_optional = detail::make_optional{};

}


#endif

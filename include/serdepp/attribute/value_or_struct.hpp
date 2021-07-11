#pragma once

#ifndef __SERDEPP_ATTRIBUTE_VALUE_OR_STRUCT__
#define __SERDEPP_ATTRIBUTE_VALUE_OR_STRUCT__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    namespace detail{
        struct value_or_struct_se {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{
                using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
                if (Helper::is_struct(ctx.adaptor)) { next_attr.from(ctx, data, key, remains...);}
                else                                { next_attr.from(ctx, data, "", remains...);}
            }

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{
                next_attr.into(ctx, data, key, remains...);
            }
        };
    }
    inline constexpr auto value_or_struct = detail::value_or_struct_se{};
}


#endif

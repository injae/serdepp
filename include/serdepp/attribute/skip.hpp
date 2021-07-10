#pragma once

#ifndef __SERDEPP_ATTRIBUTE_SKIP_HPP__
#define __SERDEPP_ATTRIBUTE_SKIP_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    namespace detail {
        struct skip {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{}

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{}
        };

        struct skip_de {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{
            next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
            }

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{}
        };

        struct skip_se {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{}

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx &ctx, T &data, std::string_view key,
                                       Next &&next_attr, Attributes &&...remains) const{
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
            }
        };


        struct skip_if_null_attr {
            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void from(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const{
                using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
                if constexpr (meta::is_optional_v<T>) {
                    if (Helper::is_null(ctx.adaptor, key)) return;
                    if(!data) data.emplace();
                    next_attr.template from<type::opt_e<std::remove_reference_t<T>>,
                                            serde_ctx>(ctx, *data, key, remains...);
                } else {
                    next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
                }
            }

            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void into(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const{
                if constexpr (meta::is_optional_v<T>) {
                    if(data) next_attr.template into<type::opt_e<std::remove_reference_t<T>>,
                                                     serde_ctx>(ctx, *data, key, remains...);
                } else {
                    next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
                }
            }
        };
    }

    inline constexpr static auto skip = detail::skip{};
    inline constexpr static auto skip_de = detail::skip_de{};
    inline constexpr static auto skip_se = detail::skip_se{};
    inline constexpr static auto skip_if_null = detail::skip_if_null_attr{};
}

#endif

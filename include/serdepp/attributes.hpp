#pragma once

#ifndef __SERDEPP_ATTRIBUTE_HPP__
#define __SERDEPP_ATTRIBUTE_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute {
    template<typename D>
    struct default_se {
        D&& default_value_;
        explicit default_se(D&& default_value) noexcept : default_value_(std::move(default_value)) {}
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_null(ctx.adaptor, key)) {
                data = std::move(default_value_);
            } else {
                next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };
    // deduce guide
    template<typename D> default_se(D&&) -> default_se<D>;

    struct value_or_struct_se {
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

    template<typename SFunc, typename DFunc>
    struct enum_hook {
        SFunc&& serialize_;
        DFunc&& deserialize_;
        explicit enum_hook(SFunc&& se, DFunc&& de) noexcept : serialize_(std::move(se))
                                                            , deserialize_(std::move(de)) {}

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            if constexpr(meta::is_enumable_v<T>) {
                std::string buffer;
                next_attr.template from<std::string, serde_ctx>(ctx, buffer, key, remains...);
                std::transform(buffer.begin(), buffer.end(), buffer.begin(), serialize_);
                data = type::enum_t::from_str<T>(buffer);
            } else {
                throw serde::enum_error("enum_hook requried enum_type");
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            if constexpr(meta::is_enumable_v<T>) {
                auto buffer = std::string{type::enum_t::to_str(data)};
                std::transform(buffer.begin(), buffer.end(), buffer.begin(), deserialize_);
                next_attr.template into<std::string, serde_ctx>(ctx, buffer, key, remains...);
            } else {
                throw serde::enum_error("enum_hook requried enum_type");
            }
        }
    };

    struct enum_toupper : enum_hook<decltype(::toupper), decltype(::tolower)> {
        enum_toupper() noexcept : enum_hook(::toupper, ::tolower) {}
    };

    struct enum_tolower : enum_hook<decltype(::tolower), decltype(::toupper)> {
        enum_tolower() noexcept : enum_hook(::tolower, ::toupper) {}
    };

    struct make_optional {
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if constexpr (meta::is_emptyable_v<T>) {
                if(Helper::is_null(ctx.adaptor, key)) {
                    data = std::move(T{});
                } else {
                    next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
                }
            } else {
                throw serde::attribute_error(fmt::format("this {} is not emptyable type",
                                                         nameof::nameof_short_type<T>()));
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            if(!data.empty()) {
                next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
            }
        }
    };

    struct skip {
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {}

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {}
    };
    
    struct skip_de {
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
        }
    };

    struct skip_se {
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {}

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };

}



#endif

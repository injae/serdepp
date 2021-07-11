#pragma once

#ifndef __SERDEPP_ATTRIBUTE_STRING_CONVERT_HPP__
#define __SERDEPP_ATTRIBUTE_STRING_CONVERT_HPP__

#include "serdepp/serializer.hpp"
#include "serdepp/attribute/skip.hpp"
#include "serdepp/attribute/algorithm.hpp"
#include "serdepp/attribute/meta.hpp"

namespace serde::attribute {
    namespace detail {
        template<class ET>
        struct str_enum {
            str_enum(ET& enum_data) : enum_data_(enum_data) {};
            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void from(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const{
                if constexpr(meta::is_enumable_v<ET>) {
                    enum_data_ = type::enum_t::from_str<ET>(data);
                } else {
                    throw serde::enum_error("this attribute requried enum type");
                }
            }

            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void into(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const{
                if constexpr(meta::is_enumable_v<ET>) {
                    data = type::enum_t::to_str(enum_data_);
                    next_attr.template into<std::string, serde_ctx>(ctx, data, key, remains...);
                } else {
                    throw serde::enum_error("this attribute requried enum type");
                }
            }

            template<typename T, typename serde_ctx>
            inline void from(serde_ctx& ctx, T& data, std::string_view key) {
                if constexpr(meta::is_enumable_v<ET>) {
                    enum_data_ = type::enum_t::from_str<ET>(data);
                } else {
                    throw serde::enum_error("this attribute requried enum type");
                }
            }

            template<typename T, typename serde_ctx>
            inline void into(serde_ctx& ctx, T& data, std::string_view key) const{
                if constexpr(meta::is_enumable_v<ET>) {
                    data = type::enum_t::to_str(enum_data_);
                } else {
                    throw serde::enum_error("this attribute requried enum type");
                }
            }
            ET& enum_data_;
        };

        struct to_upper {
            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void from(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const {
                if constexpr (meta::is_optional_v<T>) {
                    skip_if_null.from(ctx, data, key, to_upper{}, next_attr, remains...);
                }
                else if constexpr(meta::is_enumable_v<T>) {
                    std::string buffer;
                    next_attr.template from<std::string, serde_ctx>(ctx, buffer, key, remains...,
                                                                    transform{::tolower, ::toupper}, str_enum{data});
                }
                else if constexpr(meta::is_str_v<T>) {
                  if constexpr (is_serializer_call_v<Next>) {
                    next_attr.from(ctx, data, key, transform{::tolower, ::toupper}, remains...);
                  } else {
                    next_attr.from(ctx, data, key, remains..., transform{::tolower, ::toupper});
                  }
                }
                else {
                    throw serde::enum_error("this attribute requried str or enum type");
                }
            }

            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) const{
                if constexpr (meta::is_optional_v<T>) {
                    skip_if_null.into(ctx, data, key, to_upper{}, next_attr, remains...);
                }
                else if constexpr(meta::is_enumable_v<T>) {
                    std::string buffer;
                    str_enum{data}.into(ctx, buffer, key, transform{::tolower, ::toupper}, next_attr, remains...);
                }
                else if constexpr(meta::is_str_v<T>) {
                    std::string buffer = data;
                    transform{::tolower, ::toupper}.into(ctx, buffer, key, next_attr , remains...);
                }
                else {
                    throw serde::enum_error("this attribute requried str or enum type");
                }
            }
        };

        struct to_lower {
            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void from(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const {
                if constexpr (meta::is_optional_v<T>) {
                    skip_if_null.from(ctx, data, key, to_lower{}, next_attr, remains...);
                }
                else if constexpr(meta::is_enumable_v<T>) {
                    std::string buffer;
                    next_attr.template from<std::string>(ctx, buffer, key, remains...,
                                                         transform{::toupper, ::tolower}, str_enum{data});
                }
                else if constexpr(meta::is_str_v<T>) {
                  if constexpr (is_serializer_call_v<Next>) {
                    next_attr.from(ctx, data, key, transform{::toupper, ::tolower}, remains...);
                  } else {
                    next_attr.from(ctx, data, key, remains..., transform{::toupper, ::tolower});
                  }
                }
                else {
                    throw serde::enum_error("this attribute requried str or enum type");
                }
            }

            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) const{
                if constexpr (meta::is_optional_v<T>) {
                    skip_if_null.into(ctx, data, key, to_lower{}, next_attr, remains...);
                }
                else if constexpr(meta::is_enumable_v<T>) {
                    std::string buffer;
                    str_enum{data}.template into<std::string, serde_ctx>(ctx, buffer, key,
                                                                         transform{::toupper, ::tolower},
                                                                         next_attr, remains...);
                }
                else if constexpr(meta::is_str_v<T>) {
                    std::string buffer = data;
                    transform{::toupper, ::tolower}.into(ctx, data, key, next_attr, remains...);
                }
                else {
                    throw serde::enum_error("this attribute requried str or enum type");
                }
            }
        };

        
        inline int to_under(int _c){ return _c == '-' ? '_' : _c; };
        inline int to_dash(int _c) { return _c == '_' ? '-' : _c; };

        struct under_to_dash {
            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void from(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const {
                if constexpr (meta::is_optional_v<T>) {
                    skip_if_null.from(ctx, data, key, under_to_dash{}, next_attr, remains...);
                }
                else if constexpr(meta::is_enumable_v<T>) {
                    std::string buffer;
                    next_attr.template from<std::string, serde_ctx>(ctx, buffer, key, remains...,
                                                                    transform{to_under, to_dash}, str_enum{data});
                }
                else if constexpr(meta::is_str_v<T>) {
                    if constexpr(is_serializer_call_v<Next>) {
                        next_attr.from(ctx, data, key, transform{to_under, to_dash}, remains...);
                    } else {
                        next_attr.from(ctx, data, key, remains..., transform{to_under, to_dash});
                    }
                }
                else {
                    throw serde::enum_error("this attribute requried str or enum type");
                }
            }

            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) const{
                if constexpr (meta::is_optional_v<T>) {
                    skip_if_null.into(ctx, data, key, under_to_dash{}, next_attr, remains...);
                }
                else if constexpr(meta::is_enumable_v<T>) {
                    std::string buffer;
                    str_enum{data}.template into<std::string, serde_ctx>(ctx, buffer, key,
                                                                         transform{to_under, to_dash},
                                                                         next_attr, remains...);
                }
                else if constexpr(meta::is_str_v<T>) {
                    std::string buffer = data;
                    transform{to_under, to_dash}.into(ctx, buffer, key, next_attr , remains...);
                }
                else {
                    throw serde::enum_error("this attribute requried str or enum type");
                }
            }
        };
    }

    constexpr static auto to_upper = detail::to_upper{};
    constexpr static auto to_lower = detail::to_lower{};
    constexpr static auto under_to_dash = detail::under_to_dash{};
}

#endif

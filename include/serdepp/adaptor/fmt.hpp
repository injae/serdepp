#pragma once

#ifndef __SERDEPP_ADAPTOR_FMT_HPP__
#define __SERDEPP_ADAPTOR_FMT_HPP__

#include <serdepp/serializer.hpp>
#include <fmt/ranges.h>
#include <fmt/format.h>

namespace serde {
    using namespace fmt::literals;
    struct string_converter {
        struct _element {
            std::string_view id;
            std::string value;
            explicit _element(std::string_view id, const std::string& value) noexcept : id(id), value(value) {}
            std::string to_str() const {
                return id.empty() ? value : "{} : \"{}\""_format(id, value);
            }
        };
        std::vector<_element> iter;
        template<typename T>
        string_converter& add(std::string_view key, const T& data) {
            iter.push_back(_element(key, fmt::format("{}", data)));
            return *this;
        }
        std::string to_str() const { return iter.size() == 1 ? fmt::format("{}", iter[0]) : fmt::format("{}", iter); }
    };

    template<> struct serde_adaptor_helper<string_converter>: derive_serde_adaptor_helper<string_converter> {
        inline constexpr static bool is_null(string_converter& adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(string_converter& adaptor) { return 1; }
        inline constexpr static bool is_struct(string_converter& adaptor) { return true; }
    };

    template<typename T, typename U> struct serde_adaptor<string_converter, T, U> {
        static void from(string_converter& s, std::string_view key, T& data){
            throw serde::unimplemented_error(fmt::format("serde_adaptor<{}>::from(string_converter, key data)",
                                                         nameof::nameof_short_type<string_converter>()));
        }
        static void into(string_converter& s, std::string_view key, const T& data) { s.add(key, data); }
    };

    template<typename T>
    inline std::string to_string(const T& type) { return serialize<string_converter>(type).to_str(); }
    template<typename T>
    inline std::string to_str(const T& type) { return serialize<string_converter>(type).to_str(); }
}

template <typename T>
struct fmt::formatter<T, std::enable_if_t<serde::type::is_struct_v<T>, char>>
    : fmt::formatter<std::string> {
    template <typename format_ctx>
    auto format(const T& serde_type, format_ctx& ctx) {
        return formatter<std::string>::format(serde::serialize<serde::string_converter>(serde_type).to_str(), ctx);
    }
};

template<>
struct fmt::formatter<serde::string_converter> : fmt::formatter<std::string> {
    template <typename format_ctx>
    auto format(const serde::string_converter& serde_type, format_ctx& ctx) {
        return formatter<std::string>::format(serde_type.to_str(), ctx);
    }
};

template<>
struct fmt::formatter<serde::string_converter::_element> : fmt::formatter<std::string> {
    template <typename format_ctx>
    auto format(const serde::string_converter::_element& serde_type, format_ctx& ctx) {
        return formatter<std::string>::format(serde_type.to_str(), ctx);
    }
};

#endif

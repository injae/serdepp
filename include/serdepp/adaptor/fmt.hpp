#pragma once

#ifndef __SERDEPP_ADAPTOR_FMT_HPP__

#define __SERDEPP_ADAPTOR_FMT_HPP__

#include <serdepp/serializer.hpp>
#include <fmt/ranges.h>
#include <fmt/format.h>

namespace serde {
    using namespace fmt::literals;
    struct literal {
        std::vector<std::string> iter;
        template<typename T>
        literal& add(std::string_view key, const T& data) {
            iter.push_back("{}: {}"_format(key, data));
            return *this;
        }
        std::string to_string() {
            return fmt::format("{}", iter);
        }
    };

    template<> struct serde_adaptor_helper<literal>: derive_serde_adaptor_helper<literal> {
        inline constexpr static bool is_null(literal& adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(literal& adaptor) { return 1; }
        inline constexpr static bool is_struct(literal& adaptor) { return true; }
    };

    template<typename T> struct serde_adaptor<literal, T, type::struct_t> {
        static void from(literal& s, std::string_view key, T& data) {
            throw serde::unimplemented_error(fmt::format("serde_adaptor<{}>::from(literal, key data)",
                                                         nameof::nameof_short_type<literal>()));
        }
        static void into(literal& s, std::string_view key, const T& data) {
            s.add(key, deserialize<serde::literal>(data).to_string());
        }
    };

    template<typename T, typename U> struct serde_adaptor<literal, T, U> {
        static void from(literal& s, std::string_view key, T& data){
            throw serde::unimplemented_error(fmt::format("serde_adaptor<{}>::from(literal, key data)",
                                                         nameof::nameof_short_type<literal>()));
        }
        static void into(literal& s, std::string_view key, const T& data) { s.add(key, data); }
    };
}

template <typename T>
struct fmt::formatter<T, std::enable_if_t<serde::type::is_struct_v<T>, char>>
    : fmt::formatter<std::string> {
    template <typename format_ctx>
    auto format(const T& serde_type, format_ctx& ctx) {
        return formatter<std::string>::format(serde::deserialize<serde::literal>(serde_type).to_string(), ctx);
    }
};
#endif

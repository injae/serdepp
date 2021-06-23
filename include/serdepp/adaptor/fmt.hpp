#pragma once

#ifndef __SERDEPP_ADAPTOR_FMT_HPP__
#define __SERDEPP_ADAPTOR_FMT_HPP__

#include <serdepp/serializer.hpp>
#include <fmt/ostream.h>
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
        bool contains(std::string_view key)  { return true; }
        size_t size() { return 1; }
    };

    template<> struct serde_adaptor_helper<literal>: default_serde_adaptor_helper<literal> {};

    template<typename T> struct serde_adaptor<literal, T, type::struct_t> {
        static void from(literal& s, std::string_view key, T& data) { /*unimplemented*/ }
        static void into(literal& s, std::string_view key, const T& data) {
            s.add(key, deserialize<serde::literal>(data).to_string());
        }
    };

    template<typename T, typename U> struct serde_adaptor<literal, T, U> {
        static void from(literal& s, std::string_view key, T& data){ /*unimplemented*/ }
        static void into(literal& s, std::string_view key, const T& data) { s.add(key, data);  }
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

#pragma once

#ifndef __SERDEPP_ADAPTOR_FMT_HPP__
#define __SERDEPP_ADAPTOR_FMT_HPP__

#include <serdepp/adaptor/sstream.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

template <typename T>
struct fmt::formatter<T, std::enable_if_t<serde::type::is_struct_v<T>, char>>
    : fmt::formatter<std::string> {
    template <typename FormatCtx>
    auto format(const T& serde_type, FormatCtx& ctx) {
        return fmt::formatter<std::string>::format(serde::to_string(serde_type), ctx);
    }
};
//
//template<>
//struct fmt::formatter<serde::serde_sstream> : fmt::formatter<std::string> {
//    template <typename FormatCtx>
//    auto format(const serde::serde_sstream& serde_type, FormatCtx& ctx) {
//        return fmt::formatter<std::string>::format(serde_type.str(), ctx);
//    }
//};

#endif

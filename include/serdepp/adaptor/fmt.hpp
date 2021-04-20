#pragma once

#ifndef __SERDEPP_ADAPTOR_FMT_HPP__
#define __SERDEPP_ADAPTOR_FMT_HPP__

#include <serdepp/serializer.hpp>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/format.h>

namespace serde {
    struct to_string {

    };

}

template <typename t>
struct fmt::formatter<t, std::enable_if_t<serde::is_serdeable_v<serde::to_string, t>, char>>
    : fmt::formatter<std::string> {
    template <typename format_ctx> auto format(const t& a, format_ctx& ctx) {
        return fmt::formatter<std::string>::format("fmt\n", ctx);
    }
};
#endif

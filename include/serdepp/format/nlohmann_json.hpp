#pragma once

#ifndef __SERDE_FORMAT_NLOHMANN_JSON_HPP__
#define __SERDE_FORMAT_NLOHMANN_JSON_HPP__

#include "nlohmann/json.hpp"
#include "serdepp/serializer.hpp"
#include <algorithm>
#include <iterator>

namespace serde {
    using nlohmann::json;
    template<typename T> struct serde_adaptor<json,T> {
        static auto from(json& s, const std::string& key) { return s[key].template get<T>(); }
        static void to(json& s, const std::string& key, T& data) { s[key] = data; }
        static bool is(json &s, const std::string &key) { return s.contains(key); }
    };

    template<> struct serde_adaptor<json,structure<json>> {
        static auto from(json& s, const std::string& key) { return structure<json>{s[key]}; }
        static void to(json &s, const std::string &key, structure<json>& data) { s[key] = data.format; } 
        static bool is(json &s, const std::string &key) { return s.contains(key); }
    };
}

#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

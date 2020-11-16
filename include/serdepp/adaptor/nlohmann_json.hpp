#pragma once

#ifndef __SERDE_FORMAT_NLOHMANN_JSON_HPP__
#define __SERDE_FORMAT_NLOHMANN_JSON_HPP__

#include <nlohmann/json.hpp>
#include "serdepp/serializer.hpp"

namespace serde {
    using nlohmann::json;
    template<> struct serde_adaptor<json,json> {
        static auto from(json& s, const std::string& key, json& data) { data = s[key]; }
        static void   to(json &s, const std::string &key, json& data) { s[key] = data; } 
    };

    template<typename T> struct serde_adaptor<json, T>  {
        static auto from(json& s, const std::string& key, T& data) { data = s[key].template get<T>(); }
        static void   to(json& s, const std::string& key, T& data) { s[key] = data; }
    };

    template<typename T> struct serde_adaptor<json, T, type::seq> {
        using E = meta::is_sequence_e<T>;
        static auto from(json& s, const std::string& key, T& arr) {
            for(auto& value: s[key]) {
                if constexpr(is_struct<E>()) { arr.push_back(serialize<E>(value)); }
                else                         { arr.push_back(value.get<E>()); }
            }
        }
        static void   to(json& s, const std::string& key, T& data) {
            json arr;
            for(auto& value: data) {
                if constexpr(is_struct<E>()) { arr.push_back(deserialize<json>(value));}
                else                         { arr.push_back(value);}
            }
            s[key] = arr;
        }
    };

    template <typename Map> struct serde_adaptor<json, Map, type::map> {
        using T = meta::is_map_e<Map>;
        static void from(json& s, const std::string &key, Map& map) {
            for(auto&[key_, value_] : s[key].items()) {
                if constexpr(is_struct<T>()) { map[key] = serialize<T>(value_); }
                else                         { map[key] = value_.get<T>(); }
            }
        }
        static void to(json &s, const std::string& key, Map& data) {
            json map;
            for(auto& [key_, value] : data) {
                if constexpr(is_struct<T>()) { map[key_] = deserialize<json>(value); }
                else                         { map[key_] = value; }
            }
            s[key] = map;
        }
    };
}

#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

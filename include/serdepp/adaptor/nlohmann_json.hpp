#pragma once

#ifndef __SERDE_FORMAT_NLOHMANN_JSON_HPP__
#define __SERDE_FORMAT_NLOHMANN_JSON_HPP__

#define JSON_HAS_CPP_17 // nlohmann_json string_view support macro

#include <nlohmann/json.hpp>
#include "serdepp/serializer.hpp"
#include <fstream>

namespace serde {
    using nlohmann::json;
    template<> struct serde_adaptor_helper<json> : default_serde_adaptor_helper<json> {
        static json parse_file(const std::string& path) {
            json table;
            std::ifstream i(path);
            i >> table;
            return table;
        }
    };

    template<typename T>
    struct serde_adaptor<json, T>  {
        constexpr static void from(json& s, std::string_view key, T& data) {
            data = key.empty() ? s.get<T>() : s[std::string{key}].template get<T>();
        }

        constexpr static void into(json& s, std::string_view key, T& data) {
            (key.empty() ? s : s[std::string{key}]) = data;
        }
    };

    template<typename T>
    struct serde_adaptor<json, T, type::struct_t> {
        static void from(json& s, std::string_view key, T& data) {
            serialize_to(s[std::string{key}], data);
        }
        static void into(json& s, std::string_view key, T& data) {
            s[std::string{key}] = deserialize<json>(data);
        } 
    };

    template<typename T>
    struct serde_adaptor<json, T, type::seq_t> {
       using E = type::seq_e<T>;
       static void from(json& s, std::string_view key, T& arr) {
            auto& table = key.empty() ? s : s[std::string{key}];
            arr.reserve(table.size());
            for(auto& value : table) { arr.push_back(serialize<E>(value)); }
       }

       static void into(json& s, std::string_view key, T& data) {
            json arr;
            for(auto& value: data) { arr.push_back(deserialize<json>(value)); }
            (key.empty() ? s : s[std::string {key}]) = std::move(arr);
       }
    };

    template <typename Map>
    struct serde_adaptor<json, Map, type::map_t> {
        using T = type::map_e<Map>;
        static void from(json& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s[std::string{key}];
            for(auto&[key_, value_] : table.items()) { map[key_] = serialize<T>(value_); }
        }
        static void into(json& s, std::string_view key, Map& data) {
            json map;
            for(auto& [key_, value] : data) { map[key_] = deserialize<json>(value); }
            (key.empty() ? s : s[std::string{key}]) = map;
        }
    };
}

#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

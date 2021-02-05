#pragma once

#ifndef __SERDE_FORMAT_NLOHMANN_JSON_HPP__
#define __SERDE_FORMAT_NLOHMANN_JSON_HPP__

#include <nlohmann/json.hpp>
#include "serdepp/serializer.hpp"
#include <fstream>

namespace serde {
    using nlohmann::json;
    template<> struct serde_adaptor_helper<json> : default_serde_adaptor_helper<json> {
        static bool is_struct(json& s) { return s.is_object(); }
        static json parse_file(const std::string& path) {
            json table;
            std::ifstream i(path);
            i >> table;
            return table;
        }
    };

    template<typename T> struct serde_adaptor<json, T, type::struct_t> {
        static void from(json& s, const std::string& key, T& data) { serialize_to<T>(s[key], data, key);}
        static void into(json& s, const std::string& key, T& data) { s[key] = deserialize<json>(data, key); } 
    };


    template<typename T> struct serde_adaptor<json, T>  {
        static void from(json& s, const std::string& key, T& data) {
            data = key.empty() ? s.get<T>() : s[key].template get<T>();
        }
        static void into(json& s, const std::string& key, T& data) { (key.empty() ? s : s[key]) = data; }
    };

    template<typename T> struct serde_adaptor<json, T, type::seq> {
        using E = meta::is_sequence_e<T>;
        static void from(json& s, const std::string& key, T& arr) {
            auto& table = key.empty() ? s : s[key];
            arr.reserve(table.size());
            for(auto& value: table) { arr.push_back(serialize<E>(value, key)); }
        }
        static void into(json& s, const std::string& key, T& data) {
            json arr;
            for(auto& value: data) { arr.push_back(deserialize<json>(value, key)); }
            (key.empty() ? s : s[key]) = std::move(arr);
        }
    };

    template <typename Map> struct serde_adaptor<json, Map, type::map> {
        using T = meta::is_map_e<Map>;
        static void from(json& s, const std::string &key, Map& map) {
            auto& table = key.empty() ? s : s[key];
            for(auto&[key_, value_] : table.items()) { map[key] = serialize<T>(value_, key_); }
        }
        static void into(json &s, const std::string& key, Map& data) {
            json map;
            for(auto& [key_, value] : data) { map[key_] = deserialize<json>(value, key_); }
            (key.empty() ? s : s[key]) = map;
        }
    };
}
// You should use void as a second template argument
// if you don't need compile-time checks on T
#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

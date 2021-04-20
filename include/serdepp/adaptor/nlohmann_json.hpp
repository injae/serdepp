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


    template<typename T>
    struct serde_adaptor<json, T>  {
        static void from(json& s, std::string_view key, T& data) {
            data = key.empty() ? s.get<T>() : s[std::string{key}].template get<T>();
        }

        static void into(json& s, std::string_view key, T& data) {
            (key.empty() ? s : s[std::string{key}]) = data;
        }
    };

    template<typename T>
    struct serde_adaptor<json, T, type::struct_t> {
        static void from(json& s, std::string_view key, T& data) {
            serialize_to(s[std::string{key}], data, key);
        }
        static void into(json& s, std::string_view key, T& data) {
            s[std::string{key}] = deserialize<json>(data, key);
        } 
    };

    template<typename T>
    struct serde_adaptor<json, T, type::seq_t> {
       using E = type::seq_e<T>;
       static void from(json& s, std::string_view key, T& arr) {
           std::string skey{key};
           auto& table = key.empty() ? s : s[skey];
           arr.reserve(table.size());
           for(auto& value: table) { arr.push_back(serialize<E>(value, key)); }
       }

       static void into(json& s, std::string_view key, T& data) {
           std::string skey{key}; json arr;
           for(auto& value: data) { arr.push_back(deserialize<json>(value, key)); }
           (key.empty() ? s : s[skey]) = std::move(arr);
       }
    };

    template <typename Map>
    struct serde_adaptor<json, Map, type::map_t> {
        using T = type::map_e<Map>;
        static void from(json& s, std::string_view key, Map& map) {
            std::string skey{key};
            auto& table = key.empty() ? s : s[skey];
            for(auto&[key_, value_] : table.items()) { map[key_] = serialize<T>(value_, key_); }
        }

        static void into(json& s, std::string_view key, Map& data) {
            std::string skey{key}; json map;
            for(auto& [key_, value] : data) { map[key_] = deserialize<json>(value, key_); }
            (key.empty() ? s : s[skey]) = map;
        }
    };
}



#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

#pragma once

#ifndef __SERDE_FORMAT_NLOHMANN_JSON_HPP__
#define __SERDE_FORMAT_NLOHMANN_JSON_HPP__

#define JSON_HAS_CPP_17 // nlohmann_json string_view support macro

#include <nlohmann/json.hpp>
#include "serdepp/serializer.hpp"
#include <fstream>

namespace serde {
    using nlohmann::json;
    template<> struct serde_adaptor_helper<json> : derive_serde_adaptor_helper<json> {
        static json parse_file(const std::string& path) {
            json table;
            std::ifstream i(path);
            i >> table;
            return table;
        }
        inline static bool is_null(json& adaptor, std::string_view key) { return !adaptor.contains(key); }
        inline static size_t size(json& adaptor)    { return adaptor.size(); }
        inline static bool is_struct(json& adaptor) { return adaptor.is_object(); }
    };

    template<typename T>
    struct serde_adaptor<json, T>  {
        constexpr static void from(json& s, std::string_view key, T& data) {
            key.empty() ? s.get_to<T>(data) : s[std::string{key}].get_to<T>(data);
        }

        constexpr static void into(json& s, std::string_view key, const T& data) {
            (key.empty() ? s : s[std::string{key}]) = data;
        }
    };

    template<typename T>
    struct serde_adaptor<json, T, type::struct_t> {
        static void from(json& s, std::string_view key, T& data) {
            serialize_to(s[std::string{key}], data);
        }
        static void into(json& s, std::string_view key, const T& data) {
            s[std::string{key}] = deserialize<json>(data);
        } 
    };

    template<typename T>
    struct serde_adaptor<json, T, type::seq_t> {
       using E = type::seq_e<T>;
       static void from(json& s, std::string_view key, T& arr) {
           auto& table = key.empty() ? s : s.at(std::string{key});
           if constexpr(is_arrayable_v<T>) arr.reserve(table.size());
           for(auto& value : table) { arr.push_back(std::move(serialize<E>(value))); }
       }

       static void into(json& s, std::string_view key, const T& data) {
           json& arr = key.empty() ? s : s[std::string {key}];
           for(auto& value: data) { arr.push_back(std::move(deserialize<json>(value))); }
       }
    };

    template <typename Map>
    struct serde_adaptor<json, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(json& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s.at(std::string{key});
            for(auto& [key_, value_] : table.items()) { serialize_to<E>(value_, map[key_]); }
        }
        inline static void into(json& s, std::string_view key, const Map& data) {
            json& map = key.empty() ? s : s[std::string{key}];
            for(auto& [key_, value] : data) { deserialize_from<json>(value, map[key_]); }
        }
    };

    //template <typename K, typename E>
    //struct serde_adaptor<json, std::unordered_map<K,E>, type::map_t> {
    //    using Map = std::unordered_map<K,E>;
    //    static void from(json& s, std::string_view key, Map& map) {
    //        auto& table = key.empty() ? s : s[std::string{key}];
    //        map.reserve(table.size());
    //        for(auto& [key_, value_] : table.items()) { serialize_to<E>(value_, map[key_]); }
    //    }
    //    static void into(json& s, std::string_view key, const Map& data) {
    //        json map;
    //        for(auto& [key_, value] : data) { map[key_] = deserialize<json>(value); }
    //        (key.empty() ? s : s[std::string{key}]) = std::move(map);
    //    }
    //};
}

#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

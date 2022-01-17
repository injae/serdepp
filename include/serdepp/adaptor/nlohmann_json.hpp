#pragma once

#ifndef __SERDE_FORMAT_NLOHMANN_JSON_HPP__
#define __SERDE_FORMAT_NLOHMANN_JSON_HPP__

#define JSON_HAS_CPP_17 // nlohmann_json string_view support macro

#include <nlohmann/json.hpp>
#include "serdepp/serializer.hpp"
#include <fstream>

namespace serde {
    template<> struct serde_type_checker<nlohmann::json> {
        using Format = nlohmann::json;
        static bool is_integer(Format& format) { return format.is_number(); }
        static bool is_sequence(Format& format) { return format.is_array(); }
        static bool is_map(Format& format) { return format.is_object(); }
        static bool is_float(Format& format) { return format.is_number_float(); }
        static bool is_string(Format& format) { return format.is_string(); }
        static bool is_bool(Format& format) { return format.is_boolean(); }
        static bool is_null(Format& format) { return format.is_null(); }
        static bool is_struct(Format& format) { return format.is_object(); }
    };

    using nlohmann::json;
    template<> struct serde_adaptor_helper<json> : derive_serde_adaptor_helper<json> {
        static json parse_file(const std::string& path) {
            json table;
            std::ifstream i(path);
            i >> table;
            return table;

        }
        inline static bool is_null(json& adaptor, std::string_view key) { return !adaptor.contains(std::string{key}); }
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

    template<typename... T>
    struct serde_adaptor<json, std::variant<T...>>  {
        constexpr static void from(json& s, std::string_view key, std::variant<T...>& data) {
            if(key.empty()) {
                serde_variant_iter<json, std::variant<T...>, T...>(s, data);
            } else {
                serde_variant_iter<json, std::variant<T...>, T...>(s[std::string{key}], data);
            }
        }
        constexpr static void into(json& s, std::string_view key, const std::variant<T...>& data) {
            std::visit([&](auto& type){ serialize_to<json>(type, s, key); }, data);
        }
    };

    template<typename T>
    struct serde_adaptor<json, T, type::struct_t> {
        static void from(json& s, std::string_view key, T& data) {
            deserialize_to(s[std::string{key}], data);
        }
        static void into(json& s, std::string_view key, const T& data) {
            s[std::string{key}] = serialize<json>(data);
        } 
    };

    template<typename T>
    struct serde_adaptor<json, T, type::seq_t> {
       using E = type::seq_e<T>;
       static void from(json& s, std::string_view key, T& arr) {
           auto& table = key.empty() ? s : s.at(std::string{key});
           if constexpr(is_arrayable_v<T>) arr.reserve(table.size());
           for(auto& value : table) { arr.push_back(std::move(deserialize<E>(value))); }
       }

       static void into(json& s, std::string_view key, const T& data) {
           json& arr = key.empty() ? s : s[std::string{key}];
           for(auto& value: data) { arr.push_back(std::move(serialize<json>(value))); }
       }
    };

    template<typename E>
    struct serde_adaptor<json, std::vector<E>, type::seq_t> {
        static void from(json& s, std::string_view key, std::vector<E>& arr) {
           auto& table = key.empty() ? s : s.at(std::string{key});
           arr.reserve(table.size());
           for(auto& value : table) { arr.push_back(std::move(deserialize<E>(value))); }
       }

       static void into(json& s, std::string_view key, const std::vector<E>& data) {
           json& arr = key.empty() ? s : s[std::string{key}];
           for(auto& value: data) { arr.push_back(std::move(serialize<json>(value))); }
       }
    };

    template <typename Map>
    struct serde_adaptor<json, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(json& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s.at(std::string{key});
            for(auto& [key_, value_] : table.items()) { deserialize_to<E>(value_, map[key_]); }
        }
        inline static void into(json& s, std::string_view key, const Map& data) {
            json& map = key.empty() ? s : s[std::string{key}];
            for(auto& [key_, value] : data) { serialize_to<json>(value, map[key_]); }
        }
    };

    template <typename K, typename E>
    struct serde_adaptor<json, std::map<K,E>, type::map_t> {
        using Map = std::map<K,E>;
        inline static void from(json& s, std::string_view key, Map& map) {
            auto& table = key.empty() ? s : s.at(std::string{key});
            for(auto& [key_, value_] : table.items()) { deserialize_to<E>(value_, map[key_]); }
        }
        inline static void into(json& s, std::string_view key, const Map& data) {
            json& map = key.empty() ? s : s[std::string{key}];
            for(auto& [key_, value] : data) { serialize_to<json>(value, map[key_]); }
        }
    };
}

#endif
// strt_serde -> tag_to -> strt_serde -> tag_to -> ... -> adaptor_to -> adaptor_to

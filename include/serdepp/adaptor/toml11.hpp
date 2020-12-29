#pragma once

#ifndef __SERDEPP_ADAOTOR_TOML11_HPP__
#define __SERDEPP_ADAOTOR_TOML11_HPP__

#include <toml.hpp>
#include "serdepp/serializer.hpp"


namespace serde {
    using toml_v = toml::value;
    template<> struct serde_adaptor_helper<toml_v> {
        static toml_v init() { return toml::table{}; }
        static bool contains(toml_v& s, const std::string& key) { return s.contains(key); }
        static int size(toml_v& s) { return s.size(); }
        static bool is_struct(toml_v& s) { return s.is_table(); }
        static toml_v parse_file(const std::string& path) { return toml::parse(path); }
    };

    template<typename T> struct serde_adaptor<toml_v, T, type::struct_t> {
        static void from(toml_v& s, const std::string& key, T& data) { serialize_to<T>(toml::find(s,key), data, key); }
        static void   to(toml_v &s, const std::string &key, T& data) { s[key] = deserialize<toml_v>(data, key); } 
    };

    template<typename T> struct serde_adaptor<toml_v, T>  {
        static void from(toml_v& s, const std::string& key, T& data) {
            data = key.empty() ? toml::get<T>(s) : toml::find<T>(s, key);
        }
        static void   to(toml_v& s, const std::string& key, T& data) { (key.empty() ? s : s[key]) = data; }
    };

    template<typename T> struct serde_adaptor<toml_v, T, type::seq> {
        using E = meta::is_sequence_e<T>;
        static void from(toml_v& s, const std::string& key, T& arr) {
            auto table = key.empty() ? s : s[key];
            for(auto& value : table.as_array()) { arr.push_back(serialize<E>(value, key)); }
        }
        static void   to(toml_v& s, const std::string& key, T& data) {
            toml::array arr;
            if constexpr(is_struct<E>()) {
                for(auto& value: data) { arr.push_back(deserialize<toml_v>(value, key).as_table()); }
            } else {
                for(auto& value: data) { arr.push_back(deserialize<toml_v>(value, key));  }
            }
            (key.empty() ? s : s[key]) = arr;
        }
    };

    template <typename Map> struct serde_adaptor<toml_v, Map, type::map> {
        using E = meta::is_map_e<Map>;
        static void from(toml_v& s, const std::string &key, Map& map) {
            auto table = key.empty() ? s : s[key];
            for(auto& [key_, value_] : table.as_table()) { serialize_to<E>(value_, map[key_],  key_);}
        }
        static void to(toml_v &s, const std::string& key, Map& data) {
            toml_v map;
            for(auto& [key_, value_] : data) { map[key_] = deserialize<toml_v>(value_, key_); }
            (key.empty() ? s : s[key]) = map;
        }
    };
    
    
}

#endif

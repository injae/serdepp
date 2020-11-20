#pragma once

#ifndef __SERDEPP_ADAOTOR_TOML11_HPP__
#define __SERDEPP_ADAOTOR_TOML11_HPP__

#include <toml.hpp>
#include "serdepp/serializer.hpp"

namespace serde {
    using toml_v = toml::value;
    template<> struct serde_adaptor_helper<toml_v> {
        static toml_v init() { return toml::value{}; }
        static bool contains(toml_v& s, const std::string& key) { return s.contains(key); }
        static int size(toml_v& s) { return s.size(); }
    };

    template<typename T> struct serde_adaptor<toml_v, T, type::struct_t> {
        static auto from(toml_v& s, const std::string& key, T& data) { serialize_to<T>(toml::find(s,key), data, key); }
        static void   to(toml_v &s, const std::string &key, T& data) { s[key] = deserialize<toml_v>(data, key); } 
    };

    template<typename T> struct serde_adaptor<toml_v, T>  {
        static auto from(toml_v& s, const std::string& key, T& data) { data = toml::find<T>(s, key); }
        static void   to(toml_v& s, const std::string& key, T& data) { s[key] = data; }
    };

    template<typename T> struct serde_adaptor<toml_v, T, type::seq> {
        using E = meta::is_sequence_e<T>;
        static auto from(toml_v& s, const std::string& key, T& arr) {
            if constexpr (is_struct<E>()) {
                for(auto& value : s[key].as_array()) { arr.push_back(serialize<E>(value)); }
            }
            else { arr = toml::find<T>(s, key); }
        }
        static void   to(toml_v& s, const std::string& key, T& data) {
            if constexpr(is_struct<E>()) {
                toml_v arr;
                for(auto& value: data) { arr.push_back(deserialize<toml_v>(value)); }
                s[key] = arr;
            } else {
                s[key]= toml_v(data);
            }
        }
    };

    template <typename Map> struct serde_adaptor<toml_v, Map, type::map> {
        using E = meta::is_map_e<Map>;
        static void from(toml_v& s, const std::string &key, Map& map) {
            if constexpr(is_struct<E>()) {
                for(auto&[key_, value_] : s[key].as_table()) { map[key] = serialize<E>(value_, key_); }
            }else { map = toml::find<Map>(s,key); }
        }
        static void to(toml_v &s, const std::string& key, Map& data) {
            toml_v map;
            for(auto& [key_, value] : data) {
                if constexpr(is_struct<E>()) { map[key_] = deserialize<toml_v>(value, key_); }
                else                         { map[key_] = value; }
            }
            s[key] = map;
        }
    };
    
    
}

#endif

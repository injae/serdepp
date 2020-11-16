#pragma once

#ifndef __SERDEPP_ADAOTOR_TOML11_HPP__
#define __SERDEPP_ADAOTOR_TOML11_HPP__

#include <toml.hpp>
#include "serdepp/serializer.hpp"

namespace serde {
    using toml_v = toml::value;
    template<> struct serde_adaptor<toml_v,toml_v> {
        static auto from(toml_v& s, const std::string& key, toml_v& data) { data = s[key]; }
        static void   to(toml_v &s, const std::string &key, toml_v& data) { s[key] = data; } 
    };

    template<typename T> struct serde_adaptor<toml_v, T>  {
        static auto from(toml_v& s, const std::string& key, T& data) { data = toml::find<T>(s, key); }
        static void   to(toml_v& s, const std::string& key, T& data) { s[key] = data; }
    };

    template<typename T> struct serde_adaptor<toml_v, T, type::seq> {
        using E = meta::is_sequence_e<T>;
        static auto from(toml_v& s, const std::string& key, T& arr) {
            if constexpr (is_struct<E>()) {
                for(auto& value : s[key].as_array()) {
                    arr.push_back(serialize<E>(value));
                }
            }
            else { arr = toml::find<T>(s, key); }
        }
        static void   to(toml_v& s, const std::string& key, T& data) {
            toml_v arr;
            for(auto& value: data) {
                if constexpr(is_struct<E>()) { arr.push_back(deserialize<toml_v>(value));}
                else                         { arr.push_back(value);}
            }
            s[key] = arr;
        }
    };

    template <typename Map> struct serde_adaptor<toml_v, Map, type::map> {
        using E = meta::is_map_e<Map>;
        static void from(toml_v& s, const std::string &key, Map& map) {

            if constexpr(is_struct<E>()) {
                for(auto&[key_, value_] : s[key].as_table()) {
                    if constexpr(is_struct<E>()) { map[key] = serialize<E>(value_); }
                    else                         { map[key] = toml::find<E>(value_); }
                }
            }else {
                map = toml::find<Map>(s,key); 
            }

        }
        static void to(toml_v &s, const std::string& key, Map& data) {
            toml_v map;
            for(auto& [key_, value] : data) {
                if constexpr(is_struct<E>()) { map[key_] = deserialize<toml_v>(value); }
                else                         { map[key_] = value; }
            }
            s[key] = map;
        }
    };
    
    
}

#endif

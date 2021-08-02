#pragma once

#ifndef __SERDEPP_ADAPTOR_YAML_CPP_HPP__
#define __SERDEPP_ADAPTOR_YAML_CPP_HPP__

#include <yaml-cpp/yaml.h>
#include "serdepp/serializer.hpp"

namespace serde {
    using yaml = YAML::Node;

    template<> struct serde_type_checker<YAML::Node> {
        using Format = YAML::Node;
        static bool is_integer(Format& format) { return format.IsScalar(); }
        static bool is_sequence(Format& format) { return format.IsSequence(); }
        static bool is_map(Format& format) { return format.IsMap(); }
        static bool is_float(Format& format) { return format.IsMap(); }
        static bool is_string(Format& format) { return format.IsScalar(); }
        static bool is_bool(Format& format) { return format.IsScalar(); }
        static bool is_null(Format& format) { return format.IsNull(); }
        static bool is_struct(Format& format) { return format.IsMap(); }
    };


    template<> struct serde_adaptor_helper<yaml> : derive_serde_adaptor_helper<yaml> {
        static yaml parse_file(const std::string& path) { return YAML::LoadFile(path); }
        inline static bool is_null(yaml& adaptor, std::string_view key) { return !adaptor[key.data()]; }
        inline static size_t size(yaml& adaptor)    { return adaptor.size(); }
        inline static bool is_struct(yaml& adaptor) { return adaptor.IsMap(); }
    };

    template<typename T>
    struct serde_adaptor<yaml, T>  {
        constexpr static void from(yaml& s, std::string_view key, T& data) {
            data = key.empty() ? s.as<T>() : s[key.data()].as<T>();
        }

        constexpr static void into(yaml& s, std::string_view key, const T& data) {
            if(key.empty()) { s = data; } else { s[key.data()] = data; }
        }
    };

    template<typename... T>
    struct serde_adaptor<yaml, std::variant<T...>>  {
        static void from(yaml& s, std::string_view key, std::variant<T...>& data) {
            if(key.empty()) {
                serde_variant_iter<yaml, std::variant<T...>, T...>(s, data);
            } else {
                auto map = s[std::string{key}];
                serde_variant_iter<yaml, std::variant<T...>, T...>(map, data);
            }
        }
        constexpr static void into(yaml& s, std::string_view key, const std::variant<T...>& data) {
            std::visit([&](auto& type){ serialize_to<yaml>(type, s, key); }, data);
        }
    };

    template<typename T>
    struct serde_adaptor<yaml, T, type::struct_t> {
        static void from(yaml& s, std::string_view key, T& data) {
            deserialize_to(s[std::string{key}], data);
        }
        static void into(yaml& s, std::string_view key, const T& data) {
            s[std::string{key}] = serialize<yaml>(data);
        } 
    };

    template<typename T>
    struct serde_adaptor<yaml, T, type::seq_t> {
       using E = type::seq_e<T>;
       inline static void from(yaml& s, std::string_view key, T& arr) {
           if(key.empty()) {
               if constexpr(is_arrayable_v<T>) arr.reserve(s.size());
               for(std::size_t i = 0 ; i < s.size(); ++i) { arr.push_back(deserialize<E>(s[i])); }
           } else {
               auto table = s[std::string{key}];
               if constexpr(is_arrayable_v<T>) arr.reserve(table.size());
               for(std::size_t i = 0 ; i < table.size(); ++i) { arr.push_back(deserialize<E>(table[i])); }
           }
       }

       inline static void into(yaml& s, std::string_view key, const T& data) {
           if(key.empty()) {
                for(auto& value: data) { s.push_back(serialize<yaml>(value)); }
           } else {
                for(auto& value: data) { s[std::string{key}].push_back(serialize<yaml>(value)); }
           }
       }
    };


    template <typename Map>
    struct serde_adaptor<yaml, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(yaml& s, std::string_view key, Map& map) {
            if(key.empty()) {
                for(yaml::const_iterator it = s.begin(); it!=s.end(); ++it) {
                    auto key_ = it->first, value_ = it->second;
                    deserialize_to<E>(value_, map[key_.as<std::string>()]);
                }
            } else {
                for(yaml::const_iterator it = s[std::string{key}].begin(); it!=s[std::string{key}].end(); ++it) {
                    auto key_ = it->first, value_ = it->second;
                    deserialize_to<E>(value_, map[key_.as<std::string>()]);
                }
            }
        }
        inline static void into(yaml& s, std::string_view key, const Map& data) {
            if(key.empty()) {
                for(auto& [key_, value] : data) {
                    s[key_] = serialize<yaml>(value);
                }
            } else {
                yaml map = s[std::string{key}];
                for(auto& [key_, value] : data) {
                    map[key_] = serialize<yaml>(value);
                }
            }
        }
    };
}


#endif

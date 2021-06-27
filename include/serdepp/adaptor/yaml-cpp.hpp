#pragma once

#ifndef __SERDEPP_ADAPTOR_YAML_CPP_HPP__
#define __SERDEPP_ADAPTOR_YAML_CPP_HPP__

#include <yaml-cpp/yaml.h>
#include "serdepp/serializer.hpp"

namespace serde {
    using yaml = YAML::Node;

    template<> struct serde_adaptor_helper<yaml> : derive_serde_adaptor_helper<yaml> {
        static yaml parse_file(const std::string& path) { return YAML::LoadFile(path); }
        inline static bool is_null(yaml& adaptor, std::string_view key) { return !adaptor[key.data()]; }
        inline constexpr static size_t size(yaml& adaptor)    { return adaptor.size(); }
        inline constexpr static bool is_struct(yaml& adaptor) { return adaptor.IsMap(); }
    };

    template<typename T>
    struct serde_adaptor<yaml, T>  {
        constexpr static void from(yaml& s, std::string_view key, T& data) {
            data = key.empty() ? s.as<T>() : s[key.data()].as<T>();
        }

        constexpr static void into(yaml& s, std::string_view key, const T& data) {
            (key.empty() ? s : s[key.data()]) = data;
        }
    };

    template<typename T>
    struct serde_adaptor<yaml, T, type::struct_t> {
        static void from(yaml& s, std::string_view key, T& data) {
            serialize_to(s[std::string{key}], data);
        }
        static void into(yaml& s, std::string_view key, const T& data) {
            s[std::string{key}] = deserialize<yaml>(data);
        } 
    };

    template<typename T>
    struct serde_adaptor<yaml, T, type::enum_t> {
        constexpr static void from(yaml& s, std::string_view key, T& data) {
            data = type::enum_t::from_str<T>(serialize_at<std::string>(s, key));
        }
        constexpr static void into(yaml& s, std::string_view key, const T& data) {
            (key.empty() ? s : s[std::string{key}]) = std::string{type::enum_t::to_str(data)};
        }
    };

    template<typename T>
    struct serde_adaptor<yaml, T, type::seq_t> {
       using E = type::seq_e<T>;
       static void from(yaml& s, std::string_view key, T& arr) {
            auto table = key.empty() ? s : s[std::string{key}];
            if constexpr(is_arrayable_v<T>) arr.reserve(table.size());
            for(int i = 0 ; i < table.size(); ++i) { arr.push_back(std::move(serialize<E, yaml>(table[i]))); }
       }

       static void into(yaml& s, std::string_view key, const T& data) {
            yaml arr = (key.empty() ? s : s[std::string {key}]);
            int i = 0;
            for(auto& value: data) {
                auto e = arr[i++];
                deserialize_from<yaml>(value, e);
            }
       }
    };


    template <typename Map>
    struct serde_adaptor<yaml, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(yaml& s, std::string_view key, Map& map) {
            auto table = key.empty() ? s : s[std::string{key}];
            for(yaml::const_iterator it = table.begin(); it!=table.end(); ++it) {
                auto key_ = it->first, value_ = it->second;
                serialize_to<E>(value_, map[key_.as<std::string>()]);
            }
        }
        inline static void into(yaml& s, std::string_view key, const Map& data) {
            yaml map = key.empty() ? s : s[std::string{key}];
            for(auto& [key_, value] : data) {
                auto e = map[key_];
                deserialize_from<yaml>(value, e);
            }
        }
    };
}


#endif

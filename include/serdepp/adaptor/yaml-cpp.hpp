#pragma once

#ifndef __SERDEPP_ADAPTOR_YAML_CPP_HPP__
#define __SERDEPP_ADAPTOR_YAML_CPP_HPP__

#include <yaml-cpp/yaml.h>
#include "serdepp/serializer.hpp"

namespace serde {
    using yaml = YAML::Node;
    
    template<> struct serde_adaptor<yaml,yaml> : base_adaptor<yaml> {
        static auto from(yaml& s, const std::string& key) { return s[key]; }
        static void to(yaml &s, const std::string &key, yaml& data) { s[key] = data; } 
    };

    template<typename T> struct serde_adaptor<yaml,T> : base_adaptor<yaml> {
        static auto from(yaml& s, const std::string& key) { return s[key].template as<T>(); }
        static void   to(yaml& s, const std::string& key, T& data) { s[key] = data; }
    };

    template <typename K, typename T> struct serde_adaptor<yaml, std::map<K, T>> : base_adaptor<yaml>{
        using Map = std::map<K, T>;
        static auto from(yaml& s, const std::string& key) {
            Map map;
            std::for_each(s.begin(), s.end(), [&](auto& it) {
                auto& [key, value] = it;
                if(value.IsMap()) {
                    
                }
            });
            return map;
        }
        static void   to(yaml& s, const std::string &key, Map& data) {
            for(auto& [key_, data_] : data) {
                auto t = deserialize<yaml>(data_,key_);
                //s->insert(key, t);
            }
        } 
    };
}


#endif

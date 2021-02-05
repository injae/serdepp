#pragma once

#ifndef __SERDE_FORMAT_TOMLPP_HPP__
#define __SERDE_FORMAT_TOMLPP_HPP__

#include "tomlpp/toml.hpp"
#include "serdepp/serializer.hpp"
#include <string>
#include <unordered_map>

using namespace std::literals;

using toml_ptr = std::shared_ptr<toml::table>;
namespace serde {
    template<> struct serde_adaptor_helper<toml_ptr> {
        static toml_ptr init() { return toml::make_table(); }
        static bool contains(toml_ptr& s, const std::string& key) { return s->contains(key); }
        static int size(toml_ptr& s) { return std::distance(s->begin(), s->end()); }
    };

    template<typename T> struct serde_adaptor<toml_ptr, T, type::struct_t> {
        static void from(toml_ptr& s, const std::string& key, T& data){
            auto table = s->get_table(key);  serialize_to<T>(table, data, key);
        }
        static void into(toml_ptr& s, const std::string &key, T& data) {
            s->insert(key, deserialize<toml_ptr>(data, key));
        } 
    };

    template<typename T> struct serde_adaptor<toml_ptr,T>  {
        static void from(toml_ptr& s, const std::string& key, T& data) { data = s->template get_as<T>(key).value(); }
        static void into(toml_ptr& s, const std::string& key, T& data) { s->insert(key, data); }
    };

    // nested table 
    template<typename Map>
    struct serde_adaptor<toml_ptr, Map, type::map> {
        using E = meta::is_map_e<Map>;
        static void from(toml_ptr& s, const std::string& key, Map& map) {
            for(auto& [key_, data_] : *(s->get_table(key))) {
                if constexpr(is_struct<E>()) {
                    auto t_data = data_->as_table();
                    map[key_] = serialize<E>(t_data, key_);
                }else {
                    map[key_] = toml::get_impl<E>(data_).value();
                }
            }
        }
        static void into(toml_ptr& s, const std::string &key, Map& data) {
            auto map = serde_adaptor_helper<toml_ptr>::init();
            for(auto& [key_, data_] : data) {
                map->insert(key_, deserialize<toml_ptr>(data_, key_));
            }
            s->insert(key, map);
        } 
    };

    // array
    template <class T> struct serde_adaptor<toml_ptr, T, type::seq> {
        using E = meta::is_sequence_e<T>;
        static void from(toml_ptr& s, const std::string& key, T& arr) {
            if constexpr (is_struct<T>()) {
                auto tables = s->get_table_array(key);
                for(auto& it : *tables) {
                    auto data = serialize<E>(it);
                    arr.emplace_back(data);
                }
            } else { arr = s->get_array_of<E>(key).value(); }
        }
        static void into(toml_ptr& s, const std::string& key, T& data) {
            if constexpr (is_struct<E>()) {
                auto arr = toml::make_table_array();
                for(auto& it : data) { arr->push_back(deserialize<toml_ptr>(it)); }
                s->insert(key, arr);
            }else {
                auto arr = toml::make_array();
                for(auto& it : data) { arr->push_back(it); }
                s->insert(key, arr);
            }
        }
    };
}

#endif

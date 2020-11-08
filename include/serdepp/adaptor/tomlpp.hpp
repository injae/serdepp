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

    template<> struct serde_adaptor<toml_ptr,toml_ptr> : base_adaptor<toml_ptr> {
        static auto from(toml_ptr& s, const std::string& key) { return s->get_table(key); }
        static void   to(toml_ptr& s, const std::string &key, toml_ptr& data) { s->insert(key, data); } 
    };

    template<typename T> struct serde_adaptor<toml_ptr,T> : base_adaptor<toml_ptr> {
        static auto from(toml_ptr& s, const std::string& key) { return s->template get_as<T>(key).value(); }
        static void   to(toml_ptr& s, const std::string& key, T& data) { s->insert(key, data); }
    };

    // nested table 
    template <typename K, typename T>
    struct serde_adaptor<toml_ptr, std::unordered_map<K, T>> : base_adaptor<toml_ptr> {
        using Map = std::unordered_map<K, T>;
        static auto from(toml_ptr& s, const std::string& key) {
            Map map;
            for(auto& [key_, data_] : *(s->get_table(key))) {
                if(data_->is_table()) {
                    auto t_data = data_->as_table();
                    map[key_] = serialize<T>(t_data, key_);
                }else {
                    map[key_] = toml::get_impl<T>(data_).value();
                }
            }
            return map;
        }
        static void   to(toml_ptr& s, const std::string &key, Map& data) {
            for(auto& [key_, data_] : data) {
                auto t = deserialize<toml_ptr>(data_,key_);
                s->insert(key, t);
            }
        } 
    };

    template <typename K, typename T> struct serde_adaptor<toml_ptr, std::map<K, T>> : base_adaptor<toml_ptr>{
        using Map = std::map<K, T>;
        static auto from(toml_ptr& s, const std::string& key) {
            Map map;
            for(auto& [key_, data_] : *(s->get_table(key))) {
                if(data_->is_table()) {
                    auto t_data = data_->as_table();
                    map[key_] = serialize<T>(t_data, key_);
                }else {
                    map[key_] = toml::get_impl<T>(data_).value();
                }
            }
            return map;
        }
        static void   to(toml_ptr& s, const std::string &key, Map& data) {
            for(auto& [key_, data_] : data) {
                auto t = deserialize<toml_ptr>(data_,key_);
                s->insert(key, t);
            }
        } 
    };

    // array
    template <typename T> struct serde_adaptor<toml_ptr, std::vector<T>> : base_adaptor<toml_ptr> {
        using Array = std::vector<T>;
        static auto from(toml_ptr& s, const std::string& key) {
            Array arr;
            if constexpr (is_struct<T>()) {
                auto tables = s->get_table_array(key);
                for(auto& it : *tables) {
                    auto data = serialize<T>(it);
                    arr.emplace_back(data);
                }
            } else { return s->get_array_of<T>(key).value(); }
        }
        static void   to(toml_ptr& s, const std::string& key, Array& data) {
            if constexpr (is_struct<T>()) {
                auto arr = toml::make_table_array();
                for(auto& it : data) {
                    auto t = deserialize<toml_ptr>(it);
                    arr->push_back(t);
                }
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

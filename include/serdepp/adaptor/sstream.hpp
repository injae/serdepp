#pragma once

#ifndef __SERDEPP_ADAPTOR_SSTREAM_HPP__
#define __SERDEPP_ADAPTOR_SSTREAM_HPP__

#include <serdepp/serializer.hpp>
#include <sstream>

namespace serde {
    struct serde_sstream {
        std::vector<std::string> members;
        char begin_, end_;
        bool is_struct;
        serde_sstream(char begin = '{', char end = '}') : members(), begin_(begin), end_(end), is_struct(false) {}
        void set_wrapper(char begin, char end) { begin_ = begin; end_ = end; }
        serde_sstream &add(const std::string &data, std::string_view key = "") {
            members.push_back(key.empty() ? data : "\"" + std::string{key} + "\": " + data);
            return *this;
        }

        inline std::string str() const {
            std::string str;
            if (!is_struct && members.size() == 1) {
                str = members.front();
            } else {
                str = begin_ + str;
                for (auto &it : members) { str.append(it + ", "); }
                str.pop_back();
                str.back() = end_;
            }
            return str;
        }
    };

    template<>
    struct serde_adaptor_helper<serde_sstream> : derive_serde_adaptor_helper<serde_sstream> {
        inline constexpr static bool is_null(serde_sstream &adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(serde_sstream &adaptor) { return 1; }
        inline constexpr static bool is_struct(serde_sstream &adaptor) { return true; }
    };

    template<typename T> struct serde_adaptor<serde_sstream, T> {
        static void from(serde_sstream& s, std::string_view key, T& data){
            throw serde::unimplemented_error("serde_adaptor<{}>::from(serde_sstream, key data)"
                                             + std::string(nameof::nameof_short_type<serde_sstream>()));
        }
        static void into(serde_sstream& s, std::string_view key, const T& data) {
            std::stringstream ss; ss << data;
            s.add(ss.str(), key);
        }
    };

    template<typename T> struct serde_adaptor<serde_sstream, T, type::struct_t> {
        inline static void from(serde_sstream& s, std::string_view key, T& data) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        inline static void into(serde_sstream &s, std::string_view key, const T& data) {
            s.add(serialize<serde_sstream>(data).str(), key);
        } 
    };

    template<typename... T>
    struct serde_adaptor<serde_sstream, std::variant<T...>>  {
        constexpr static void from(serde_sstream& s, std::string_view key, std::variant<T...>& data) {
            throw serde::unimplemented_error("serde_adaptor<{}>::from(serde_sstream, key data)");
        }
        constexpr static void into(serde_sstream& s, std::string_view key, const std::variant<T...>& data) {
            std::visit([&](auto& type){ serialize_to(type, s, key); }, data);
        }
    };

    template<typename T> struct serde_adaptor<serde_sstream, T, type::seq_t> {
        static void from(serde_sstream& s, std::string_view key, T& arr) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        static void into(serde_sstream& s, std::string_view key, const T& data) {
            if(key.empty()) {
                s.set_wrapper('[', ']');
                for(auto& it : data) { s.add(serialize<serde_sstream>(it).str()); }
            } else {
                serde_sstream ss('[',']');
                for(auto& it : data) { ss.add(serialize<serde_sstream>(it).str()); }
                s.add(ss.str(), key);
            }
        }
    };

    template <typename Map> struct serde_adaptor<serde_sstream, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(serde_sstream& s, std::string_view key, Map& map) {
            throw serde::unimplemented_error("serde_adaptor::from(serde_sstream, key data)");
        }
        inline static void into(serde_sstream& s, std::string_view key, const Map& data) {
            if(key.empty()) {
                for(auto& [key_, it] : data) { s.add(serialize<serde_sstream>(it).str(), key_); }
            } else {
                serde_sstream ss;
                for(auto& [key_, it] : data) { ss.add(serialize<serde_sstream>(it).str(), key_); }
                s.add(ss.str(), key);
            }
        }
    };

    template<typename T>
    inline std::string to_string(const T& type) { return serialize<serde_sstream>(type).str(); }
    template<typename T>
    inline std::string to_str(const T& type) { return serialize<serde_sstream>(type).str(); }
}

#endif

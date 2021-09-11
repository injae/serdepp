#pragma once

#ifndef __SERDEPP_ADAPTOR_RAPID_JSON_HPP__
#define __SERDEPP_ADAPTOR_RAPID_JSON_HPP__

#include "serdepp/serializer.hpp"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <fstream>

namespace serde {
    using rapidjson_type = rapidjson::Document;

    template<> struct serde_type_checker<rapidjson_type> {
        using Format = rapidjson_type;
        static bool is_integer(Format& format) { return format.IsInt(); }
        static bool is_sequence(Format& format) { return format.IsArray(); }
        static bool is_map(Format& format) { return format.IsObject(); }
        static bool is_float(Format& format) { return format.IsFloat(); }
        static bool is_string(Format& format) { return format.IsString(); }
        static bool is_bool(Format& format) { return format.IsBool(); }
        static bool is_null(Format& format) { return format.IsNull(); }
        static bool is_struct(Format& format) { return format.IsObject(); }
    };

    template<> struct serde_type_checker<rapidjson::Value> {
        using Format = rapidjson::Value;
        static bool is_integer(Format& format) { return format.IsInt(); }
        static bool is_sequence(Format& format) { return format.IsArray(); }
        static bool is_map(Format& format) { return format.IsObject(); }
        static bool is_float(Format& format) { return format.IsFloat(); }
        static bool is_string(Format& format) { return format.IsString(); }
        static bool is_bool(Format& format) { return format.IsBool(); }
        static bool is_null(Format& format) { return format.IsNull(); }
        static bool is_struct(Format& format) { return format.IsObject(); }
    };

    template<> struct serde_adaptor_helper<rapidjson_type> : derive_serde_adaptor_helper<rapidjson_type> {
        static rapidjson_type parse_file(const std::string& path) {
            using namespace rapidjson;
            Document doc;
            std::ifstream ifs{path};
            IStreamWrapper isw{ifs};
            doc.ParseStream(isw);
            return doc;
        }
        inline static bool is_null(rapidjson_type& adaptor, std::string_view key) {
            return !adaptor.HasMember(key.data());
        }
        inline static size_t size(rapidjson_type& adaptor)    {
            return std::distance(adaptor.GetObject().begin(), adaptor.GetObject().end());
        }
        inline static bool is_struct(rapidjson_type& adaptor) { return adaptor.IsObject(); }
    };

    template<> struct serde_adaptor_helper<rapidjson::Value> : derive_serde_adaptor_helper<rapidjson::Value> {
        inline static bool is_null(rapidjson::Value& adaptor, std::string_view key) {
            return !adaptor.HasMember(key.data());
        }
        inline static size_t size(rapidjson::Value& adaptor)    {
            return std::distance(adaptor.GetObject().begin(), adaptor.GetObject().end());
        }
        inline static bool is_struct(rapidjson::Value& adaptor) { return adaptor.IsObject(); }
    };

    template<typename T>
    struct serde_adaptor<rapidjson_type, T>  {
        constexpr static void from(rapidjson_type& s, std::string_view key, T& data) {
            data = key.empty() ? s.Get<T>() : s[key.data()].Get<T>(); 
        }

        static void into(rapidjson_type& s, std::string_view key, const T& data) {
            if(key.empty()) {
                s.Set(data);
            } else {
                if(!s.IsObject()) s.SetObject();
                rapidjson::Value v;
                v.Set(data);
                s.AddMember(rapidjson::StringRef(key.data()), v.Move(), s.GetAllocator());
            }
        }
    };

    template<typename T>
    struct serde_adaptor<rapidjson::Value, T> {
        static void from(rapidjson::Value& s, std::string_view key, T& data) {
            data = key.empty() ? s.Get<T>() : s[key.data()].Get<T>(); 
        }
    };

    template<>
    struct serde_adaptor<rapidjson::Value, std::string>  {
        static void from(rapidjson::Value& s, std::string_view key, std::string& data) {
            data = key.empty() ? s.GetString() : s[key.data()].GetString(); 
        }
    };


    template<>
    struct serde_adaptor<rapidjson_type, std::string>  {
        static void from(rapidjson_type& s, std::string_view key, std::string& data) {
            data = key.empty() ? s.GetString() : s[key.data()].GetString(); 
        }

        static void into(rapidjson_type& s, std::string_view key, const std::string& data) {
            if(key.empty()) {
                s.SetString(data.c_str(), data.length(), s.GetAllocator());
            } else {
                if(!s.IsObject()) s.SetObject();
                rapidjson::Value buf;
                buf.SetString(data.c_str(), data.length(), s.GetAllocator());
                s.AddMember(rapidjson::StringRef(key.data()), buf.Move(), s.GetAllocator());
            }
        }
    };

    template<typename... T>
    struct serde_adaptor<rapidjson_type, std::variant<T...>>  {
        constexpr static void from(rapidjson_type& s, std::string_view key, std::variant<T...>& data) {
            if(key.empty()) {
                serde_variant_iter<rapidjson_type, std::variant<T...>, T...>(s, data);
            } else {
                serde_variant_iter<rapidjson::Value, std::variant<T...>, T...>(s[key.data()], data);
            }
        }
        constexpr static void into(rapidjson_type& s, std::string_view key, const std::variant<T...>& data) {
            std::visit([&](auto& type){ serialize_to<rapidjson_type>(type, s, key); }, data);
        }
    };

    template<typename T>
    struct serde_adaptor<rapidjson_type, T, type::struct_t> {
        static void from(rapidjson_type& s, std::string_view key, T& data) {
            using namespace rapidjson;
            auto& obj = s[key.data()];
            if(!obj.IsObject()) obj.SetObject();
            deserialize_to(obj, data);
        }
        static void into(rapidjson_type& s, std::string_view key, const T& data) {
            using namespace rapidjson;
            Value buf;
            buf.CopyFrom(serialize<rapidjson_type>(data).Move(), s.GetAllocator());
            s.AddMember(rapidjson::StringRef(key.data()), buf.Move(), s.GetAllocator());
        } 
    };

    template<typename T>
    struct serde_adaptor<rapidjson::Value, T, type::struct_t> {
        static void from(rapidjson::Value& s, std::string_view key, T& data) {
            using namespace rapidjson;
            auto& obj = s[key.data()];
            if(!obj.IsObject()) obj.SetObject();
            deserialize_to(obj, data);
        }
    };
    
    template<typename T>
    struct serde_adaptor<rapidjson_type, T, type::seq_t> {
        using E = type::seq_e<T>;
        static void from(rapidjson_type& s, std::string_view key, T& arr) {
            using namespace rapidjson;
            auto& table = key.empty() ? s : s[key.data()];
            if(!table.IsArray()) { table.SetArray(); }
            if constexpr(is_arrayable_v<T>) arr.reserve(table.Size());
            for(auto i = 0; i < table.Size(); ++i) {
                arr.push_back(std::move(deserialize<E>(table[i])));
            }
        }
        
        static void into(rapidjson_type& s, std::string_view key, const T& data) {
            using namespace rapidjson;
            auto& alloc = s.GetAllocator();
            Value arr;
            arr.SetArray();
            arr.Reserve(data.size(), alloc);
            for(auto& value: data) {
                Value v;
                v.CopyFrom(serialize<rapidjson_type>(value), alloc);
                arr.PushBack(v.Move(), alloc);
            }
            if(key.empty()) {
                s.CopyFrom(arr.Move(), alloc);
            }else {
                if(!s.IsObject()) s.SetObject();
                s.AddMember(StringRef(key.data()), arr.Move(), alloc);
            } 
        }
    };

    template<typename T>
    struct serde_adaptor<rapidjson::Value, T, type::seq_t> {
        using E = type::seq_e<T>;
        static void from(rapidjson::Value& s, std::string_view key, T& arr) {
            using namespace rapidjson;
            auto& table = key.empty() ? s : s[key.data()];
            if(!table.IsArray()) { table.SetArray(); }
            if constexpr(is_arrayable_v<T>) arr.reserve(table.Size());
            for(auto i = 0; i < table.Size(); ++i) {
                arr.push_back(std::move(deserialize<E>(table[i])));
            }
        }
    };
    
    template <typename Map>
    struct serde_adaptor<rapidjson_type, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(rapidjson_type& s, std::string_view key, Map& map) {
            using namespace rapidjson;
            auto& table = key.empty() ? s : s[key.data()];
            for(auto& [key_, value_] : table.GetObject()) {
                std::string k = key_.GetString();
                deserialize_to<E>(value_, map[k]);
            }
        }
        inline static void into(rapidjson_type& s, std::string_view key, const Map& data) {
            using namespace rapidjson;
            Value map;
            map.SetObject();
            for(auto& [key_, value_] : data) {
                Value val;
                val.CopyFrom(serialize<rapidjson_type>(value_), s.GetAllocator());
                map.AddMember(StringRef(key_.c_str(), key_.length()), val.Move(), s.GetAllocator());
            }
            if(key.empty()) {
                s.CopyFrom(map.Move(), s.GetAllocator());
            } else {
                if(!s.IsObject()) s.SetObject();
                s.AddMember(StringRef(key.data()), map.Move(), s.GetAllocator());
            }
        }
    };


    template <typename Map>
    struct serde_adaptor<rapidjson::Value, Map, type::map_t> {
        using E = type::map_e<Map>;
        inline static void from(rapidjson::Value& s, std::string_view key, Map& map) {
            using namespace rapidjson;
            auto& table = key.empty() ? s : s[key.data()];
            for(auto& [key_, value_] : table.GetObject()) {
                std::string k = key_.GetString();
                deserialize_to<E>(value_, map[k]);
            }
        }
    };
}


#endif

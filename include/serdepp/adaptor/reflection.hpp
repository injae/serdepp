#pragma once

#ifndef __SERDEPP_ADAPTOR_RFLECTION_HPP__
#define __SERDEPP_ADAPTOR_RFLECTION_HPP__

#include <serdepp/serde.hpp>
#include <string_view>

namespace serde {
    template<class T>
    struct reflect_from_name {
        //DERIVE_SERDE(reflect<T>,(&Self::key, "key")(&Self::member, "member"))
        reflect_from_name() = default;
        reflect_from_name(std::string_view member) : key(member) {}
        std::string_view key;
        T* member = nullptr;
    };

    template<class T, size_t idx>
    struct reflect_from_index {
        constexpr static size_t find_idx = idx;
        //DERIVE_SERDE(reflect<T>,(&Self::key, "key")(&Self::member, "member"))
        reflect_from_index() = default;
        size_t index = 0;
        T* member = nullptr;
    };

    template<class T>
    struct serde_adaptor_helper<reflect_from_name<T>> : derive_serde_adaptor_helper<reflect_from_name<T>> {
        inline constexpr static bool is_null(reflect_from_name<T> &adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(reflect_from_name<T> &adaptor) { return 1; }
        inline constexpr static bool is_struct(reflect_from_name<T> &adaptor) { return true; }
    };

    template<class T, size_t idx>
    struct serde_adaptor_helper<reflect_from_index<T,idx>> : derive_serde_adaptor_helper<reflect_from_index<T, idx>> {
        inline constexpr static bool is_null(reflect_from_index<T, idx> &adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(reflect_from_index<T, idx> &adaptor) { return 1; }
        inline constexpr static bool is_struct(reflect_from_index<T, idx> &adaptor) { return true; }
    };

    template<typename U, typename T>
    struct serde_adaptor<reflect_from_name<U>, T, std::enable_if_t<std::is_same_v<U, remove_cvref_t<T>>>> {
        constexpr static void from(reflect_from_name<U>& s, std::string_view key, T& data){
            if(key == s.key) {
                s.member = &data;
            }
        }
        static void into(reflect_from_name<U>& s, std::string_view key, const T& data) {
            throw serde::unimplemented_error("serde_adaptor<reflect_from_name>::into(reflect_from_name, key, data)");
        }
    };

    template<typename U, typename T>
    struct serde_adaptor<reflect_from_name<U>, T , std::enable_if_t<!std::is_same_v<U, remove_cvref_t<T>>>> {
        static void from(reflect_from_name<U>& s, std::string_view key, T& data){
            if(key == s.key) {
                std::string error = std::string(nameof::nameof_type<U>());
                throw serde::type_error(error + " != " + std::string(nameof::nameof_type<remove_cvref_t<T>>()));
            }
        }
        static void into(reflect_from_name<U>& s, std::string_view key, const T& data) {
            throw serde::unimplemented_error("serde_adaptor<reflect_from_name>::into(reflect_from_name, key, data)");
        }
    };

    template<typename U, typename T, size_t index>
    struct serde_adaptor<reflect_from_index<U, index>, T, std::enable_if_t<std::is_same_v<U, remove_cvref_t<T>>>> {
        constexpr static void from(reflect_from_index<U, index>& s, std::string_view key, T& data){
            if(s.index == s.find_idx) {
                s.member = &data;
            }
            s.index++;
        }
        static void into(reflect_from_index<U,index>& s, std::string_view key, const T& data) {
            throw serde::unimplemented_error("serde_adaptor<reflect_from_name>::into(reflect_from_name, key, data)");
        }
    };
    
    template<typename U, typename T, size_t index>
    struct serde_adaptor<reflect_from_index<U, index>, T, std::enable_if_t<!std::is_same_v<U, remove_cvref_t<T>>>> {
        static void from(reflect_from_index<U, index>& s, std::string_view key, T& data){
            if(s.index == s.find_idx) {
                std::string error = std::string(nameof::nameof_type<U>());
                throw serde::type_error(error + " != " + std::string(nameof::nameof_type<remove_cvref_t<T>>()));
            }
            s.index++;
        }
        static void into(reflect_from_index<U,index>& s, std::string_view key, const T& data) {
            throw serde::unimplemented_error("serde_adaptor<reflect_from_name>::into(reflect_from_name, key, data)");
        }
    };
    

    template<typename M, class T>
    constexpr M& mem_get(T& type, std::string_view member_name) {
        auto ref = serde::reflect_from_name<M>(member_name);
        serde::deserialize_to(ref, type);
        if(ref.member == nullptr) {
            throw serde::type_error("can't find member: " + std::string(member_name));
        }
        return *ref.member;
    }

    template<typename M, size_t idx, class T>
    constexpr M& mem_get(T& type) {
        auto ref = serde::reflect_from_index<M,idx>();
        serde::deserialize_to(ref, type);
        if(ref.member == nullptr) {
            throw serde::type_error("out of index: " + std::to_string(idx));
        }
        return *ref.member;
    }
}

#endif

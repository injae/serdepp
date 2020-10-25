#pragma once

#ifndef __CPPSER_META_HPP__
#define __CPPSER_META_HPP__

#include <iostream>
#include <typeinfo>
#include <list>
#include <variant>
#include <functional>
#include <algorithm>
#include <map>
#include <unordered_map>

namespace serde::meta {
    template<typename T, typename U>
    constexpr inline T only_str(U&& type) {
        if constexpr (std::is_same_v<T,std::string>) { return std::string{type};} else {return type;}
    }

    inline void panic(bool opt, const std::string& message) {
        if(!opt) {
            std::cerr << message << std::endl;
            exit(1);
        }
    }
    template<typename T>
    inline auto panic(std::shared_ptr<T> ptr, const std::string& message) {
        if(!ptr) {
            std::cerr << message << std::endl;
            exit(1);
        }
        return ptr;
    }

    inline std::string toml_error_meesage(const std::string& name, const std::string type="table", const std::string parent ="") {
        auto p = (parent != "") ? parent +"." : "";
        return "[toml parse error] Require " + type + ": -> " + p + name;
    }


    template<class T> struct is_vector {
        using type = T;
        using element = T;
        constexpr static bool value = false;
    };

    template<class T>
    struct is_vector<std::vector<T>> {
        using type = std::vector<T>;
        using element = T;
        constexpr  static bool value = true;
    };

    template< typename T> inline constexpr bool is_vector_v = is_vector<T>::value;
    template< typename T> using is_vector_t = typename is_vector<T>::type;
    template< typename T> using is_vector_e = typename is_vector<T>::element;

    template<class T> struct is_list {
        using type = T;
        using element = T;
        constexpr static bool value = false;
    };

    template<class T>
    struct is_list<std::list<T>> {
        using type = std::list<T>;
        using element = T;
        constexpr  static bool value = true;
    };

    template< typename T> inline constexpr bool is_list_v = is_list<T>::value;
    template< typename T> using is_list_t = typename is_list<T>::type;
    template< typename T> using is_list_e = typename is_list<T>::element;

    template<class T> struct is_optional {
        using type = T;
        using element = T;
        constexpr static bool value = false;
    };

    template<class T> struct is_optional<std::optional<T>> {
        using type = std::optional<T>;
        using element = T;
        constexpr static bool value = true;
    };

    template< typename T> inline constexpr bool is_optional_v = is_vector<T>::value;
    template< typename T> using is_optional_t = typename is_optional<T>::type;
    template< typename T> using is_optional_e = typename is_optional<T>::element;


    template<class T>
    struct is_map {
        using type = T;
        using key = T;
        using element = T;
        constexpr static bool value = false;
    };

    template<class T, class U>
    struct is_map<std::map<T,U>> {
        using type = std::map<T,U>;
        using key = T;
        using element = U;
        constexpr static bool value = true;
    };

    template<class T, class U>
    struct is_map<std::unordered_map<T,U>> {
        using type = std::unordered_map<T,U>;
        using key = T;
        using element = U;
        constexpr static bool value = true;
    };

    template< typename T> inline constexpr bool is_map_v = is_map<T>::value;
    template< typename T> using is_map_t = typename is_map<T>::type;
    template< typename T> using is_map_k = typename is_map<T>::key;
    template< typename T> using is_map_e = typename is_map<T>::element;
    
    template <typename T, typename = void> struct is_iterable : std::false_type {};
    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                      decltype(std::end(std::declval<T>()))>> : std::true_type {};
    template <typename T> constexpr bool is_iterable_v = is_iterable<T>::value;
    
    namespace detail {
        struct type_interface {
            template<typename T> using opt = std::optional<T>;
            template<typename T> using arr = std::vector<T>;
            template<typename T> using nested = std::map<std::string,T>;
            friend class access;
        };

        struct table : public type_interface {};

    } // namespace detail
}

#endif

#pragma once

#ifndef __SERDEPP_META_HPP__
#define __SERDEPP_META_HPP__

#include <string>
#include <string_view>

#include <vector>
#include <list>
#include <optional>
#include <iostream>
#include <deque>
#include <unordered_map>
#include <map>

namespace serde::meta {
    template <typename T, typename = void> struct is_iterable : std::false_type {};
    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                        decltype(std::end(std::declval<T>()))>> : std::true_type {};
    template<typename T> constexpr bool is_iterable_v = is_iterable<T>::value;
    template<typename T> constexpr auto remove_const(T& type) { return static_cast<std::remove_const_t<T>>(type); }

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
        constexpr static bool value = true;
    };

    template< typename T> inline constexpr bool is_list_v = is_list<T>::value;
    template< typename T> using is_list_t = typename is_list<T>::type;
    template< typename T> using is_list_e = typename is_list<T>::element;

    template<class T>
    struct is_sequence {
        using type = T;
        using element = T;
        constexpr  static bool value = false;
    };
    template< typename T> inline constexpr bool is_sequence_v = is_sequence<T>::value;
    template< typename T> using is_sequence_t = typename is_sequence<T>::type;
    template< typename T> using is_sequence_e = typename is_sequence<T>::element;

    template<class T>
    struct is_map {
        using type = T;
        using key = T;
        using element = T;
        constexpr static bool value = false;
    };

    template<typename T> inline constexpr bool is_map_v = is_map<T>::value;
    template<typename T> using is_map_t = typename is_map<T>::type;
    template<typename T> using is_map_k = typename is_map<T>::key;
    template<typename T> using is_map_e = typename is_map<T>::element;

    template<class T>
    struct is_literal {
        using type = T;
        using traits = T;
        constexpr static bool value = false;
    };

    template<class T, class Traits, class Alloc>
    struct is_literal<std::basic_string<T, Traits, Alloc>> {
        using type = T;
        using traits = Traits;
        using alloc = Alloc;
        constexpr static bool value = true;
    };

    template<class T, class Traits>
    struct is_literal<std::basic_string_view<T, Traits>> {
        using type = T;
        using traits = Traits;
        constexpr static bool value = true;
    };
}

#endif

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <typeinfo>
#include <set>
#include <list>
#include <type_traits>
#include <utility>
#include <fmt/format.h>
#include <string_view>

#ifndef __SERDEPP_META_HPP__
#define __SERDEPP_META_HPP__

namespace serde::meta {
    template <typename T, typename = void> struct is_iterable : std::false_type {};
    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
        : std::true_type {};
    template<class T> constexpr auto is_iterable_v = is_iterable<T>::value;

    template <typename T, typename = void> struct is_emptyable : std::false_type {};

    template <typename T>
    struct is_emptyable<T, std::void_t<decltype(std::declval<T>().empty())>> : std::true_type {};
    template<class T> constexpr auto is_emptyable_v = is_emptyable<T>::value;


    template<typename T, typename U = void> struct is_mappable : std::false_type { };

    template<typename T>
    struct is_mappable<T, std::void_t<typename T::key_type,
                                      typename T::mapped_type,
                                      decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>>
    : std::true_type { };
    template<typename T>  inline constexpr auto is_mappable_v = is_mappable<T>::value;


    template<typename T, typename U = void> struct is_sequenceable : std::false_type { };
    template<typename T>
    struct is_sequenceable<T, std::enable_if_t<is_iterable_v<T> && !is_mappable_v<T>>> : std::true_type { };
    template<typename T>  inline constexpr auto is_sequenceable_v = is_sequenceable<T>::value;


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
    template<typename T>  inline constexpr auto is_literal_v = is_literal<T>::value;



    template<class CTX, typename T, typename = void> struct is_serdeable : std::false_type {};
    template<class CTX, typename T>
    struct is_serdeable<CTX, T, std::void_t<decltype(std::declval<T>()
                                .template /*auto*/ serde<CTX>(/*CTX& ctx)*/
                                std::add_lvalue_reference_t<CTX>(std::declval<CTX>()),
                                std::add_lvalue_reference_t<T>(std::declval<T>())
                                                              ))> > : std::true_type {};
    template<class CTX, typename T> inline constexpr auto is_serdeable_v = is_serdeable<CTX, T>::value;

    template<typename T, typename = void> struct is_optional : std::false_type {};
    template<typename T> struct is_optional<T,std::void_t<decltype(std::declval<T>().has_value()),
                                                          decltype(std::declval<T>().value()),
                                                          decltype(*(std::declval<T>()))
                                                          >> : std::true_type {};
    template<typename T> inline constexpr auto is_optional_v = is_optional<T>::value;

    template<typename T, typename = void> struct is_optionable : std::false_type {};
    template<typename T> struct is_optionable<T, std::enable_if_t<is_emptyable_v<T>>> : std::true_type {};
    template<typename T> struct is_optionable<T, std::enable_if_t<is_optional_v<T>>> : std::true_type {};
    template<typename T> inline constexpr auto is_optionable_v = is_optionable<T>::value;
  
}

#endif

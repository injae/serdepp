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
#include <string_view>
#include <magic_enum.hpp>

#ifndef __SERDEPP_META_HPP__
#define __SERDEPP_META_HPP__

namespace serde::meta {
    template<class T>
    struct remove_cvref {
        typedef std::remove_cv_t<std::remove_reference_t<T>> type;
    };
    template< class T > using remove_cvref_t = typename remove_cvref<T>::type;

    template <class F, size_t... Is>
    constexpr auto index_apply_impl(F f, std::index_sequence<Is...>) { return f(Is...); }

    template <size_t N, class F>
    constexpr auto index_apply(F f) { return index_apply_impl(f, std::make_index_sequence<N>{}); }

    template <typename T, typename = void> struct is_iterable : std::false_type {};
    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
        : std::true_type {};
    template<class T> constexpr auto is_iterable_v = is_iterable<T>::value;

    template <typename T, typename = void> struct is_arrayable : std::false_type {};
    template <typename T>
    struct is_arrayable<T, std::void_t<decltype(std::declval<T>().begin()),
                                       decltype(std::declval<T>().end()),
                                       decltype(std::declval<T>().reserve(0))>
                        > : std::true_type {};
    template<class T> constexpr auto is_arrayable_v = is_arrayable<T>::value;

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

    template<typename T, typename U = void> struct is_enumable : std::false_type {};
    template<typename T>
    struct is_enumable<T, std::enable_if_t<magic_enum::is_scoped_enum_v<T> ||
                                           magic_enum::is_unscoped_enum_v<T>>> : std::true_type {};
    template<typename T>  inline constexpr auto is_enumable_v = is_enumable<T>::value;


    template<typename T, typename U = void> struct is_pointer : std::false_type {};
    template<typename T>
    struct is_pointer<T, std::void_t<decltype(*std::declval<T>())>> : std::true_type {};
    template<typename T>  inline constexpr auto is_pointer_v = is_pointer<T>::value;

    template<class T>
    struct is_str_type {
        using type = T;
        using traits = T;
        constexpr static bool value = false;
    };

    template<class T, class Traits, class Alloc>
    struct is_str_type<std::basic_string<T, Traits, Alloc>> {
        using type = T;
        using traits = Traits;
        using alloc = Alloc;
        constexpr static bool value = true;
    };

    template<class T, class Traits>
    struct is_str_type<std::basic_string_view<T, Traits>> {
        using type = T;
        using traits = Traits;
        constexpr static bool value = true;
    };
    template<typename T>  inline constexpr auto is_str_v = is_str_type<T>::value;


    template<typename T, typename = void> struct is_optional : std::false_type {};
    template<typename T> struct is_optional<T,std::void_t<decltype(std::declval<T>().has_value()),
                                                          decltype(std::declval<T>().value()),
                                                          decltype(*(std::declval<T>()))
                                                          >> : std::true_type {};
    template<typename T> inline constexpr auto is_optional_v = is_optional<T>::value;

    template<typename T, typename = void> struct is_default : std::false_type {};
    template<typename T> struct is_default<T, std::enable_if_t<is_emptyable_v<T>>> : std::true_type {};
    template<typename T> struct is_default<T, std::enable_if_t<is_optional_v<T>>> : std::true_type {};
    template<typename T> inline constexpr auto is_default_v = is_default<T>::value;


    template<class Format, typename T, typename = void> struct is_serdeable : std::false_type {};
    template<class Format, typename T>
    struct is_serdeable<Format, T, std::void_t<decltype(std::declval<T>().template
                                    /*auto*/ serde<Format>(/*Format& ctx)*/
                                        std::add_lvalue_reference_t<Format>(std::declval<Format>()), /*format& */
                                        std::add_lvalue_reference_t<T>(std::declval<T>()) /*value& */))>
                                     > : std::true_type {};
    template<class CTX, typename T> inline constexpr auto is_serdeable_v = is_serdeable<CTX, T>::value;


    template<typename Attribute, typename T>
    using attr = typename Attribute::template serde_attribute<T>;
}

#endif

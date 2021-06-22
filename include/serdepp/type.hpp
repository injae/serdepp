#pragma once

#include "serdepp/meta.hpp"

#ifndef __SERDEPP_TYPE_HPP__
#define __SERDEPP_TYPE_HPP__


namespace serde::type {
    using namespace serde::meta;

    struct seq_t{};
    struct map_t{};
    struct struct_t{};

    template<typename T> using map_e = typename T::mapped_type;
    template<typename T> using map_k = typename T::key_type;
    template<typename T> using seq_e = typename T::value_type;

    template<typename T, typename = void> struct not_null;

    template<typename T>
    struct not_null<T, std::enable_if_t<is_emptyable_v<T>>> {
        not_null(T& origin) : data(origin) { }
        T& data;
    };

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

    template<typename T> using is_optional_element = typename is_optional<T>::element;

    template<class T> struct is_not_null {
        using type = T;
        using element = T;
        constexpr static bool value = false;
    };


    template<class T>
    struct is_not_null<not_null<T>> {
        using type = not_null<T>;
        using element = T;
        constexpr static bool value = true;
    };

    namespace detail { struct dummy_context{}; }
    template<class T> constexpr static bool is_struct_v = is_serdeable_v<detail::dummy_context, T>;

}

#endif

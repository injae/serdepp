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
#include <memory>

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

    template<class,class>
    struct tuple_extend;

    template<class Input>
    struct tuple_extend<Input, void> {
        using type = std::tuple<Input>;
    };
    
    template<class Input, class... Args>
    struct tuple_extend<Input, std::tuple<Args...>> {
        using type = std::tuple<Args..., Input>;
    };

    template<class Input, class... Args>
    using tuple_extend_t = typename tuple_extend<Input, Args...>::type;

    template<typename... T>
    struct to_variant {
        using type = std::variant<T...>;
    };

    template<typename... T>
    struct to_variant<std::tuple<T...>> {
        using type = std::variant<T...>;
    };

    template<typename... T>
    using to_variant_t = typename to_variant<T...>::type;

    template <typename T, typename = void> struct is_iterable : std::false_type {};
    template <typename T>
    struct is_iterable<T, std::void_t<
                              decltype(std::declval<T>().begin()),
                              decltype(std::declval<T>().end())>>
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

    template <> struct is_emptyable<bool> : std::true_type {};

    template <typename T>
    struct is_emptyable<T, std::void_t<decltype(std::declval<T>().empty())>> : std::true_type {};
    template<class T> constexpr auto is_emptyable_v = is_emptyable<T>::value;


    template<typename T, typename U = void> struct is_mappable : std::false_type { };

    template<typename T>
    struct is_mappable<T, std::void_t<typename T::key_type,
                                      typename T::mapped_type,
                                      decltype(std::declval<T&>()
                                               [std::declval<const typename T::key_type&>()])>>
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

    //template<typename T, typename U = void> struct is_pointer : std::false_type {};
    //template<typename T>
    //struct is_pointer<T, std::enable_if_t<std::is_pointer_v<T>>> : std::true_type {};
    //template<typename T, typename E>
    //struct is_pointer<T, std::enable_if_t<std::is_same_v<T, std::unique_ptr<E>>>> : std::true_type {};
    //template<typename T>  inline constexpr auto is_pointer_v = is_pointer<T>::value;

    template<typename T, typename = void>
    struct is_pointer {
        using type = T;
        using null_type = void;
        constexpr static bool value = false;
    };

    template<typename T>
    struct is_pointer<T, std::enable_if_t<std::is_pointer_v<T>>> {
        using type = std::remove_pointer_t<T>;
        constexpr static std::nullptr_t null_type = nullptr;
        inline static constexpr auto init() { return new type(); }
        constexpr static bool value = true;
    };

    template<typename T>
    struct is_pointer<std::unique_ptr<T>> {
        using type = T;
        constexpr static std::nullptr_t null_type = nullptr;
        inline static constexpr auto init() { return std::make_unique<T>(); }
        constexpr static bool value = true;
    };

    template<typename T>
    struct is_pointer<std::shared_ptr<T>> {
        using type = T;
        constexpr static std::nullptr_t  null_type = nullptr;
        inline static constexpr auto init() { return std::make_shared<T>(); }
        constexpr static bool value = true;
    };

    //template<typename T>
    //struct is_pointer<std::weak_ptr<T>> {
    //    using type = T;
    //    constexpr static std::nullptr_t null_type = nullptr;
    //    inline constexpr auto init() { throw serde::unimplemented_error("weakptr not suppport"); }
    //    constexpr static bool value = true;
    //};
    
    template<typename T>  inline constexpr auto is_pointer_v = is_pointer<T>::value;
    template<typename T>  inline constexpr auto is_pointer_t = is_pointer<T>::type;


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
                                                          decltype(std::declval<T>().value())
                                                          >> : std::true_type {};
    template<typename T> inline constexpr auto is_optional_v = is_optional<T>::value;

    template<typename T, typename = void> struct is_default : std::false_type {};
    template<typename T> struct is_default<T, std::enable_if_t<is_emptyable_v<T>>> : std::true_type {};
    template<typename T> struct is_default<T, std::enable_if_t<is_optional_v<T>>> : std::true_type {};
    template<typename T> inline constexpr auto is_default_v = is_default<T>::value;


    template<class Context, typename T, typename = void> struct is_serdeable : std::false_type {};
    template<class Context, typename T>
    struct is_serdeable<Context, T, std::void_t<decltype(std::declval<T>().template
                                    /*auto*/ serde<Context>(/*Context& ctx)*/
                                        std::add_lvalue_reference_t<Context>(std::declval<Context>()), /*format& */
                                        std::add_lvalue_reference_t<T>(std::declval<T>()) /*value& */))>
                                     > : std::true_type {};
    template<class CTX, typename T> inline constexpr auto is_serdeable_v = is_serdeable<CTX, T>::value;

    template<class serde_ctx, class Target>
    constexpr const auto is_adaptor_v = std::is_same_v<typename serde_ctx::Adaptor, Target>;

    template<typename Attribute, typename T>
    using attr = typename Attribute::template serde_attribute<T>;

}

#endif

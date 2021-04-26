#pragma once

#ifndef __CPPSER_SERIALIZER_HPP__
#define __CPPSER_SERIALIZER_HPP__

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

namespace serde
{
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


    template<class CTX, typename T, typename = void> struct is_serdeable : std::false_type {};
    template<class CTX, typename T>
    struct is_serdeable<CTX, T, std::void_t<decltype(std::declval<T>()
                                .template /*auto*/ serde<CTX>(/*CTX& ctx)*/
                                std::add_lvalue_reference_t<CTX>(std::declval<CTX>())))> > : std::true_type {};
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

    struct serde_exception : std::exception {

        serde_exception(std::string_view msg) : message(msg) {};
        std::string_view message;
        const char* what() const throw() { return message.data(); }
    };

    template <class Adaptor> struct serde_adaptor_helper {
        inline constexpr static bool is_null(Adaptor &adaptor, std::string_view key);
    };

    template <class Adaptor>
    struct default_serde_adaptor_helper { // default serde adaptor helper
        inline constexpr static bool is_null(Adaptor &adaptor, std::string_view key) {
            return adaptor.contains(key);
        }
    };

    template<typename S, typename T, typename = void>
    struct serde_adaptor {
        static void from(S& s, std::string_view key, T& data);
        static void into(S& s, std::string_view key, T& data);
    };

    namespace detail {
        struct dumy_adaptor : default_serde_adaptor_helper<dumy_adaptor> {};
    };

    namespace type {
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

        template<class T> struct is_not_null {
            using type = T;
            using element = T;
            constexpr static bool value = false;
        };

        template<class T>
        struct is_not_null<not_null<T>> {
            using type = not_null<T>;
            using element = T;
            constexpr  static bool value = true;
        };
    };

    template<typename T, typename serde_ctx, typename = void>
    struct serde_serializer {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            fmt::print("normal type\n");
            serde_adaptor<Adaptor, T>::from(ctx.adaptor, key, data);
        }
        constexpr inline static auto into(serde_ctx& ctx, T&& data, std::string_view key) {
            fmt::print("normal type\n");
            serde_adaptor<Adaptor, T>::into(ctx.adaptor, key, data);
        }
    };

    template<class ADAPTOR> class serde_context {
    public:
        using Adaptor = ADAPTOR;
        serde_context(Adaptor& _adpator, std::string_view _name) :adaptor(_adpator), name(_name) {}
        Adaptor& adaptor;
        std::string_view name;

        template<typename T>
        constexpr serde_context& field(T& type, std::string_view tag) {
            fmt::print(stderr, "unknown");
            return *this;
        }

        template<typename T, typename U>
        constexpr serde_context& field(T& type, std::string_view tag, U&& default_value) {
            fmt::print(stderr, "unknown");
            return *this;
        }
    private:
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_optional_v<T>>> {
        using value_type = typename T::value_type;
        using Adaptor = typename serde_ctx::Adaptor;
        using Helper = default_serde_adaptor_helper<Adaptor>;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            fmt::print("optional=>");
            if(Helper::is_null(ctx.adaptor, key)) {
                data = std::nullopt;
            } else {
                serde_serializer<value_type, serde_ctx>::from(ctx, *data, key);
            }
        }
        constexpr inline static auto into(serde_ctx& ctx, T&& data, std::string_view key) {
            fmt::print("optional=>");
            if(data) {
                serde_serializer<value_type, serde_ctx>::into(ctx, std::forward<value_type>(*data), key);
            }
        }
        template<typename U>
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key, U&& default_value) {
            if(Helper::is_null(ctx.adaptor, key)) {
                data = default_value;
            } else {
                serde_serializer<value_type, serde_ctx>::from(ctx, *data, key);
            }
        }

        template<typename U>
        constexpr inline static auto into(serde_ctx& ctx, T&& data, std::string_view key, U&& default_value) {
            if(!data) data = default_value;
            serde_serializer<value_type, serde_ctx>::into(ctx, std::forward<value_type>(*data), key);
        }
    };


    template <typename T, class Adaptor>
    constexpr inline T serialize(Adaptor&& adaptor, std::string_view name = "") {
      using origin = typename std::remove_reference_t<Adaptor>;
      T target;
      serde_context<origin> ctx(adaptor, name);
      serde_serializer<T, serde_context<origin>>::from(ctx, target, "");
      return target;
    }

    template <typename T, class Adaptor>
    constexpr inline T serialize(Adaptor&& adaptor, T&& default_type, std::string_view name = "") {
      using origin = typename std::remove_reference_t<Adaptor>;
      T target;
      serde_context<origin> ctx(adaptor, name);
      serde_serializer<T, serde_context<origin>>::from(ctx, target, "", default_type);
      return target;
    }

    template<typename T, class Adaptor>
    constexpr inline void serialize_to(Adaptor&& adaptor, T& target, std::string_view name="") {
        using origin = typename std::remove_reference_t<Adaptor>;
        serde_context<origin> ctx(adaptor, name);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, "");
    }

    template<typename T, class Adaptor>
    constexpr inline void serialize_to(Adaptor&& adaptor, T& target, T&& default_type, std::string_view name="") {
        using origin = typename std::remove_reference_t<Adaptor>;
        serde_context<origin> ctx(adaptor, name);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, "", default_type);
    }

    template<typename T, class Adaptor>
    constexpr inline T serialize_at(Adaptor&& adaptor, std::string_view key) {
        using origin = typename std::remove_reference_t<Adaptor>;
        T target;
        serde_context<origin> ctx(adaptor, key);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
        return target;
    }

    template<typename T, class Adaptor>
    constexpr inline void serialize_at_to(Adaptor& adaptor, T& target, std::string_view name) {
        using origin = typename std::remove_reference_t<Adaptor>;
        serde_context<origin> ctx(adaptor, name);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, name);
    }


    template<class Adaptor, typename T>
    constexpr inline Adaptor deserialize(T&& target, std::string_view name="") {
        using origin = typename std::remove_reference_t<T>;
        Adaptor adaptor;
        serde_context<Adaptor> ctx(adaptor, name);
        serde_serializer<origin, serde_context<Adaptor>>::into(ctx, std::forward<origin>(target), "");
        return adaptor;
    }

    template<class Adaptor, typename T>
    constexpr inline Adaptor deserialize_by(T&& target, std::string_view key) {
        using origin = typename std::remove_reference_t<T>;
        Adaptor adaptor;
        serde_context<Adaptor> ctx(adaptor, key);
        serde_serializer<origin, serde_context<Adaptor>>::into(ctx, std::forward<origin>(target), key);
        return adaptor;
    }

    template<class Adaptor, typename T>
    constexpr inline Adaptor deserialize_by_from(Adaptor& adaptor, T&& target, std::string_view key) {
        using origin = typename std::remove_reference_t<T>;
        serde_context<Adaptor> ctx(adaptor, key);
        serde_serializer<origin, serde_context<Adaptor>>::into(ctx, std::forward<origin>(target), key);
        return adaptor;
    }

    template<class ADAPTOR, bool is_serialize> class serde_struct_context {
    public:
        using Adaptor = ADAPTOR;
        serde_struct_context(Adaptor& _adpator, std::string_view _name) : adaptor(_adpator), name(_name) {}
        Adaptor& adaptor;
        std::string_view name;

        template<typename T>
        constexpr serde_struct_context& field(T& type, std::string_view tag) {
            using origin = typename std::remove_reference_t<T>;
            if constexpr(is_serialize) {
                serialize_at_to<origin>(this->adaptor, type, tag);
            } else {
                deserialize_by_from<Adaptor>(this->adaptor, type, tag);
            }
            fmt::print("?:{}={}\n", tag, type);
            return *this;
        }

        template<typename T, typename U>
        constexpr serde_struct_context& field(T& type, std::string_view tag, U&& default_value) {
            using origin = typename std::remove_reference_t<T>;
            auto octxt = serde_context<Adaptor>(this->adaptor, tag);
            if constexpr(is_serialize) {
                serialize_at_to<origin>(this->adaptor, type, tag);
            } else {
                deserialize_by_from<Adaptor>(this->adaptor, type, tag);
            }
            fmt::print("?:{}={}\n", tag, type);
            return *this;
        }

        bool is_serialize_step() { return is_serialize; }
    };


    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_serdeable_v<serde_ctx, T>>> {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if(key.empty()) {
                serde_struct_context struct_ctx = serde_struct_context<Adaptor,true>{ctx.adaptor, key};
                data.serde(struct_ctx);
            } else {
                serde_adaptor<Adaptor, T, type::struct_t>::from(ctx.adaptor, key, data);
            }
            fmt::print("struct type\n");
        }
        constexpr inline static auto into(serde_ctx& ctx, T&& data, std::string_view key) {
            if(key.empty()) {
                auto struct_ctx = serde_struct_context<Adaptor, false>(ctx.adaptor, key);
                data.serde(struct_ctx);
            } else {
                serde_adaptor<Adaptor, T, type::struct_t>::into(ctx.adaptor, key, data);
            }
            fmt::print("struct type\n");
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_mappable_v<T> && is_emptyable_v<T>>> {
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::map_t>::from(ctx.adaptor, key, data);
            fmt::print("map type\n");
        }
        constexpr inline static auto into(serde_ctx& ctx, T&& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::map_t>::into(ctx.adaptor, key, data);
            fmt::print("map type\n");
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_sequenceable_v<T> && is_emptyable_v<T>>> {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<Adaptor, T, type::seq_t>::from(ctx.adaptor, key, data);
            fmt::print("sequence type\n");
        }
        constexpr inline static auto into(serde_ctx& ctx, T&& data, std::string_view key) {
            serde_adaptor<Adaptor, T, type::seq_t>::into(ctx.adaptor, key, data);
            fmt::print("sequence type\n");
        }
    };

} // namespace serde
#endif

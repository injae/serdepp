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
#include <variant>
#include <string_view>
#include <nameof.hpp>
#include <magic_enum.hpp>
#include <functional>
#include <algorithm>
#include "serdepp/meta.hpp"
#include "serdepp/exception.hpp"


namespace serde
{
    using namespace serde::meta;

    template <class Adaptor, typename=void> struct serde_adaptor_helper {
        // for support optional type parse
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key);
        // for support no_remain function
        inline constexpr static size_t size(Adaptor& adaptor);
        // for support string_or_struct
        inline constexpr static bool is_struct(Adaptor& adaptor);
        // for support parse_file
        inline constexpr static Adaptor parse_file(const std::string& path);
    };

    template <class Adaptor>
    struct derive_serde_adaptor_helper { // default serde adaptor helper
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::is_null(adaptor, key)");
            return false;
        }

        inline constexpr static size_t size(Adaptor& adaptor) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::size(adaptor, key)");
            return 0;
        }

        inline constexpr static bool is_struct(Adaptor& adaptor) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::is_struct(adaptor, key)");
            return false;
        }

        inline constexpr static Adaptor parse_file(const std::string& path) {
            throw serde::unimplemented_error("serde_adaptor<" +
                                             std::string(nameof::nameof_short_type<Adaptor>())
                                             +">::parse_file(adaptor, key)");
            return Adaptor{};
        }
    };

    template<typename S, typename T, typename = void>
    struct serde_adaptor {
        static void from(S& s, std::string_view key, T& data);
        static void into(S &s, std::string_view key, const T &data);
    };

    namespace detail {
        struct dummy_adaptor{};
    };

    template <> struct serde_adaptor_helper<detail::dummy_adaptor>
        : derive_serde_adaptor_helper<detail::dummy_adaptor> {
    };

    template<class T, bool is_serialize_=false>
    struct serde_context {
        using Adaptor = T;
        using Helper = serde_adaptor_helper<Adaptor>;
        constexpr static bool is_serialize = is_serialize_;
        constexpr serde_context(T& format) : adaptor(format) {}
        T& adaptor;
        size_t read_count_ = 0;
        bool skip_all_ = false; 
        constexpr void read() { read_count_++; }
    };

    namespace type {
        struct seq_t {};
        struct map_t {};
        struct struct_t {};
        struct enum_t {
            template <class Enum>
            inline constexpr static std::string_view to_str(Enum value) {
                return magic_enum::enum_name(value);
            }

            template <class Enum>
            inline constexpr static Enum from_str(std::string_view str) {
                auto value = magic_enum::enum_cast<Enum>(str);
                if (!value.has_value()) {
                    throw enum_error(std::string{nameof::nameof_type<Enum>()} + "::" + std::string{str});
                }
                return *value;
            }
        };

        template <typename T> using map_e = typename T::mapped_type;
        template <typename T> using map_k = typename T::key_type;
        template <typename T> using seq_e = typename T::value_type;
        template <typename T> using opt_e = typename T::value_type;


        template <typename T, typename = void> struct not_null;

        template <typename T>
        struct not_null<T, std::enable_if_t<is_emptyable_v<T>>> {
            not_null(T &origin) : data(origin) {}
            T &data;
        };

        template <class T> struct is_optional {
            using type = T;
            using element = T;
            constexpr static bool value = false;
        };

        template <class T> struct is_optional<std::optional<T>> {
            using type = std::optional<T>;
            using element = T;
            constexpr static bool value = true;
        };

        template <typename T> using optional_element = typename is_optional<T>::element;

        template <class T> struct is_not_null {
            using type = T;
            using element = T;
            constexpr static bool value = false;
        };

        template <class T> struct is_not_null<not_null<T>> {
            using type = not_null<T>;
            using element = T;
            constexpr static bool value = true;
        };

        template <class T> using is_struct = is_serdeable<serde_context<detail::dummy_adaptor>, T>;
        template <class T> inline constexpr auto is_struct_v = is_struct<T>::value;
    }

    template<typename T, typename serde_ctx, typename = void>
    struct serde_serializer {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if constexpr(is_serdeable_v<serde_ctx, T>) {
                if(key.empty()) {
                    serde_context struct_ctx = serde_context<Adaptor>{ctx.adaptor};
                    data.serde(struct_ctx, data);
                } else {
                    serde_adaptor<Adaptor, T, type::struct_t>::from(ctx.adaptor, key, data);
                }
            } else {
                serde_adaptor<Adaptor, std::remove_reference_t<T>>::from(ctx.adaptor, key, data);
            }
            ctx.read();
        }
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            if constexpr(is_serdeable_v<serde_ctx, T>) {
                if(key.empty()) {
                    auto struct_ctx = serde_context<Adaptor, true>(ctx.adaptor);
                    data.serde(struct_ctx, const_cast<T&>(data));
                } else {
                    serde_adaptor<Adaptor, T, type::struct_t>::into(ctx.adaptor, key, data);
                }
            } else {
                serde_adaptor<Adaptor, std::remove_reference_t<T>>::into(ctx.adaptor, key, data);
            }
            ctx.read();
        }
    };

    namespace attribute {
        namespace detail {
            struct serializer_call_attr {
                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    serde::serde_serializer<T, serde_ctx>::from(ctx, data, key);
                    next_attr.template from<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    serde::serde_serializer<T, serde_ctx>::into(ctx, data, key);
                    next_attr.template into<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx>
                inline void from(serde_ctx& ctx, T& data, std::string_view key) const {
                    serde::serde_serializer<T, serde_ctx>::from(ctx, data, key);
                }

                template<typename T, typename serde_ctx>
                inline void into(serde_ctx& ctx, T& data, std::string_view key) const { 
                    serde::serde_serializer<T, serde_ctx>::into(ctx, data, key);
                }
            };

            struct empty_attr {
                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    next_attr.template from<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
                constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                Next&& next_attr, Attributes&&... remains) const {
                    next_attr.template into<T, serde_ctx>(ctx, data, key, std::forward<Attributes>(remains)...);
                }

                template<typename T, typename serde_ctx>
                constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key) const { }

                template<typename T, typename serde_ctx>
                constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key) const { }
            };
        }
        inline constexpr auto serializer_call = detail::serializer_call_attr{};
        inline constexpr auto pass = detail::empty_attr{};


    }

    using namespace std::string_view_literals;

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      Adaptor(key) -> T or Adaptor -> T
      T t = deserialize<T>(Adaptor, key)
     */
    template <typename T, class Adaptor>
    constexpr inline T deserialize(Adaptor&& adaptor, std::string_view key="") {
        using origin = meta::remove_cvref_t<Adaptor>;
        T target;
        if constexpr(std::is_pointer_v<T>) { target = nullptr; }
        serde_context<origin> ctx(adaptor);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
        return target;
    }

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      Adaptor[key] -> T or Adaptor -> T
      deserialize_to(Adaptor, T, key)
     */
    template<typename T, class Adaptor>
    constexpr inline void deserialize_to(Adaptor&& adaptor, T& target, std::string_view key="") {
        using origin = meta::remove_cvref_t<Adaptor>;
        serde_context<origin> ctx(adaptor);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
    }

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      T -> Adaptor[key] or T -> Adaptor
      Adaptor adaptor = serialize<Adaptor>(T, key)
     */
    template<class Adaptor, typename T>
    constexpr inline Adaptor serialize(T&& target, std::string_view key="") {
        using origin = meta::remove_cvref_t<T>;
        Adaptor adaptor;
        serde_context<Adaptor, true> ctx(adaptor);
        serde_serializer<origin, serde_context<Adaptor,true>>::into(ctx, target, key);
        return adaptor;
    }

    /*
      Adaptor = [nlohmann::json serde_sstream, toml11::value ...]
      T -> Adaptor[key] or T -> Adaptor
      serialize_to(T, Adaptor, key)
     */
    template<class Adaptor, typename T, typename U = meta::remove_cvref_t<T>>
    constexpr inline void serialize_to(T&& target, Adaptor& adaptor, std::string_view key="") {
        using origin = meta::remove_cvref_t<T>;
        serde_context<Adaptor, true> ctx(adaptor);
        serde_serializer<origin, serde_context<Adaptor, true>>::into(ctx, target, key);
    }

    template<class Adaptor>
    constexpr inline Adaptor parse_file(const std::string& path) {
        return serde_adaptor_helper<Adaptor>::parse_file(path);
    }

    template<class Context, class T, class TUPLE=void>
    class serde_struct {
        using TYPE = T;
        Context& context_;
        T& value_;
        constexpr const static std::string_view type = nameof::nameof_type<T>();
    public:
        using Tuple = TUPLE;
        constexpr serde_struct(Context& context, T& value) : context_(context), value_(value) {}

        template<class MEM_PTR, typename Attribute, typename... Attributes>
        inline constexpr auto field(MEM_PTR&& ptr, std::string_view name,
                                             Attribute&& attribute, Attributes&&... attributes) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            using type_tuple = meta::tuple_extend_t<rtype, Tuple>;
            if(context_.skip_all_) return serde_struct<Context, T, type_tuple>(context_, value_);
            if constexpr(!Context::is_serialize) {
                attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::detail::serializer_call_attr{});
            } else {
                attribute.template into<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::detail::serializer_call_attr{});
            }
            return serde_struct<Context, T, type_tuple>(context_, value_);
        }

        template<class MEM_PTR>
        inline constexpr auto field(MEM_PTR&& ptr, std::string_view name) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            using type_tuple = meta::tuple_extend_t<rtype, Tuple>;
            if(context_.skip_all_) return serde_struct<Context, T, type_tuple>(context_, value_);
            if constexpr(!Context::is_serialize) {
                serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
            } else {
                serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
            }
            return serde_struct<Context, T, type_tuple>(context_, value_);
        }
        
        inline constexpr serde_struct& no_remain() {
            using namespace std::literals;
            if(context_.skip_all_) return *this;
            if constexpr (!Context::is_serialize) {
                const auto adaptor_size = Context::Helper::is_struct(context_.adaptor)
                    ? Context::Helper::size(context_.adaptor)
                    : 1;
                const auto serde_size = context_.read_count_;
                if(adaptor_size > serde_size) {
                    throw unregisted_data_error("serde["s + std::string{type} + "] read: " + std::to_string(serde_size)
                                                + " != adaptor["
                                                + std::string(nameof::nameof_short_type<decltype(context_.adaptor)>())
                                                + "] read: " + std::to_string(adaptor_size));
                }
            }
            return *this;
        }


        template<class MEM_PTR>
        inline constexpr auto operator()(MEM_PTR&& ptr, std::string_view name) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            using type_tuple = meta::tuple_extend_t<rtype, Tuple>;
            if(context_.skip_all_) return serde_struct<Context, T, type_tuple>(context_, value_);
            if constexpr(!Context::is_serialize) {
                serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
            } else {
                serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
            }
            return serde_struct<Context, T, type_tuple>(context_, value_);
        }

        template<class MEM_PTR, typename Attribute, typename... Attributes>
        inline constexpr auto operator()(MEM_PTR&& ptr, std::string_view name,
                                                  Attribute&& attribute, Attributes&&... attributes) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            using type_tuple = meta::tuple_extend_t<rtype, Tuple>;
            if(context_.skip_all_) return serde_struct<Context, T, type_tuple>(context_, value_);
            if constexpr(!Context::is_serialize) {
                attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::serializer_call);
            } else {
                attribute.template into<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::serializer_call);
            }
            return serde_struct<Context, T, type_tuple>(context_, value_);
        }


        template<typename Attribute, typename... Attributes>
        inline constexpr serde_struct& attributes(Attribute&& attribute, Attributes&&... attributes) {
            if(context_.skip_all_) return *this;
            if constexpr(!Context::is_serialize) {
                attribute.template from<T, Context>(context_, value_, "",
                                                    std::forward<meta::remove_cvref_t<Attributes>>
                                                    (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                    attribute::pass);
            } else {
                attribute.template into<T, Context>(context_, value_, "",
                                                    std::forward<meta::remove_cvref_t<Attributes>>
                                                    (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                    attribute::pass);
            }
            return *this;
        }

        template <typename... Attributes>
        struct ApplyAttribute{
            using def = serde_struct<Context, T, Tuple>;
            ApplyAttribute(def& s, std::tuple<Attributes...>&& attr) : s(s), attributes(std::move(attr)) {}
            template<class MEM_PTR>
            inline constexpr def& operator()(MEM_PTR&& ptr, std::string_view name){
                std::apply([&](Attributes&... args){ s(ptr, name, args...);}, attributes);
                return s;
            }
            template <typename... Attributess>
            inline constexpr def& operator[](std::tuple<Attributess...>&& attr) {
                std::apply([&](Attributess&... args){ s.attributes(args...); }, attr);
                return s;
            }
            def& s;
            std::tuple<Attributes...> attributes;
        };

        template <typename... Attributes>
        constexpr inline ApplyAttribute<Attributes...> operator[](std::tuple<Attributes...>&& attr) {
            return ApplyAttribute<Attributes...>(*this, std::move(attr));
        }
    };

    namespace attribute {
        template<typename... Ty>
        inline constexpr std::tuple<Ty...> attributes(Ty&&... arg) {
            return std::make_tuple(std::forward<Ty>(arg)...);
        }

        template<class Context, class T, typename... Ty>
        inline constexpr serde::serde_struct<Context, T> operator|(std::tuple<Ty...> attributes,
                                                    serde::serde_struct<Context, T> serde_define) {
            auto ptsr = [&](auto... v) {serde_define.attr(v...);};
            std::apply(ptsr, std::move(attributes));
            return std::move(serde_define);
        }
    }
    template<class Context, class T> serde_struct(Context&, T&) -> serde_struct<Context, T>;

    template<typename serde_ctx>
    struct serde_serializer<std::monostate, serde_ctx> {
        constexpr inline static auto from(serde_ctx& ctx, std::monostate& data, std::string_view key) {}
        constexpr inline static auto into(serde_ctx& ctx, const std::monostate& data, std::string_view key) {}
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_mappable_v<T> &&
                                                           is_emptyable_v<T> >> {
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::map_t>::from(ctx.adaptor, key, data);
            ctx.read();
        }
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::map_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_sequenceable_v<T> &&
                                                           is_emptyable_v<T> &&
                                                           !is_str_v<T>  >>{
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::seq_t>::from(ctx.adaptor, key, data);
            ctx.read();
        }

        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::seq_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };


    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_enumable_v<T>>>{
        inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            std::string str_to_enum;
            serde_adaptor<typename serde_ctx::Adaptor, std::string>::from(ctx.adaptor, key, str_to_enum);
            data = type::enum_t::from_str<T>(str_to_enum);
            ctx.read();
        }
        inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            auto enum_to_str = std::string{type::enum_t::to_str(data)};
            serde_adaptor<typename serde_ctx::Adaptor, std::string>::into(ctx.adaptor, key, enum_to_str);
            ctx.read();
        }
    };

    enum class SERDE_TYPE {
        NONE,
        BOOL,
        INTEGER,
        STRING,
        FLOAT,
        SEQUENCE,
        MAP,
        STRUCT,
        UNKNOWN,
        ENUM
    };

    template<typename Format, typename=void>
    struct serde_type_checker {
        static bool is_integer(Format& format)  { return true; }
        static bool is_sequence(Format& format) { return true; }
        static bool is_map(Format& format)      { return true; }
        static bool is_float(Format& format)    { return true; }
        static bool is_string(Format& format)   { return true; }
        static bool is_bool(Format& format)     { return true; }
        static bool is_null(Format& format)     { return true; }
        static bool is_struct(Format& format)   { return true; }
    };

    template<class T>
    constexpr SERDE_TYPE serde_type_declare() {
             if constexpr(meta::is_str_v<T>)          return SERDE_TYPE::STRING;
        else if constexpr(meta::is_sequenceable_v<T>) return SERDE_TYPE::SEQUENCE;
        else if constexpr(meta::is_mappable_v<T>)     return SERDE_TYPE::MAP;
        else if constexpr(meta::is_enumable_v<T>)     return SERDE_TYPE::ENUM;
        else if constexpr(type::is_struct_v<T>)       return SERDE_TYPE::STRUCT;
        else if constexpr(std::is_same_v<int16_t, T>) return SERDE_TYPE::INTEGER;
        else if constexpr(std::is_same_v<int8_t, T>)  return SERDE_TYPE::INTEGER;
        else if constexpr(std::is_same_v<int32_t, T>) return SERDE_TYPE::INTEGER;
        else if constexpr(std::is_same_v<int64_t, T>) return SERDE_TYPE::INTEGER;
        else if constexpr(std::is_same_v<bool, T>)    return SERDE_TYPE::BOOL;
        else if constexpr(std::is_same_v<float, T>)   return SERDE_TYPE::FLOAT;
        else if constexpr(std::is_same_v<double, T>)  return SERDE_TYPE::FLOAT;
        else                                          return SERDE_TYPE::UNKNOWN;
    }

    template<class Format, class T, class V>
    bool serde_variant_setter(Format& format, V& data) {
        switch(serde_type_declare<T>()) {
        case SERDE_TYPE::STRING:
            if(!serde_type_checker<Format>::is_string(format)) return true;
            break;
        case SERDE_TYPE::SEQUENCE:
            if(!serde_type_checker<Format>::is_sequence(format)) return true;
            break;
        case SERDE_TYPE::MAP:
            if(!serde_type_checker<Format>::is_map(format)) return true;
            break;
        case SERDE_TYPE::STRUCT:  
            if(!serde_type_checker<Format>::is_struct(format)) return true;
            break;
        case SERDE_TYPE::INTEGER:
            if(!serde_type_checker<Format>::is_integer(format)) return true;
            break;
        case SERDE_TYPE::BOOL:
            if(!serde_type_checker<Format>::is_bool(format)) return true;
            break;
        case SERDE_TYPE::FLOAT:
            if(!serde_type_checker<Format>::is_float(format)) return true;
            break;
        case SERDE_TYPE::ENUM:
            if(!serde_type_checker<Format>::is_string(format)) return true;
            break;
        default: return true;
        //case SERDE_TYPE::UNKNOWN:  
        }
        try {
            data = deserialize<T>(format);
            return false;
        } catch(std::exception& ex) {
            return true;
        }
    }

    template<class Format, class V, class Cur, class ...T>
    constexpr void serde_variant_iter(Format& format, V& data) {
        if constexpr (sizeof...(T) != 0) {
            bool is_not_find = serde_variant_setter<Format, Cur, V>(format, data);
            if(!is_not_find) return;
            serde_variant_iter<Format, V, T...>(format, data);
        } else {
            if(serde_variant_setter<Format,
               std::variant_alternative_t<std::variant_size_v<V>-1, V>, V>(format, data))  {
                if(!std::is_same_v<std::variant_alternative_t<0, V>, std::monostate>) {
                    throw serde::variant_error("empty variant: if you use optional variant, use std::monostate");
                }
            }
        }
    }

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_optional_v<T>>> {
        using value_type = type::opt_e<std::remove_reference_t<T>>;
        using Adaptor = typename serde_ctx::Adaptor;
        using Helper = serde_adaptor_helper<Adaptor>;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if(key.empty() && serde_type_checker<Adaptor>::is_null(ctx.adaptor)) {
                if(!data) data = std::nullopt;
            } else if (!key.empty() && Helper::is_null(ctx.adaptor, key)) {
                if(!data) data = std::nullopt;
            } else {
                if(!data) data.emplace();
                serde_serializer<value_type, serde_ctx>::from(ctx, *data, key);
          }
        }
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            if(data) { serde_serializer<value_type, serde_ctx>::into(ctx, *data, key); }
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_pointer_v<T>>> {
        using Adaptor = typename serde_ctx::Adaptor;
        using Helper = serde_adaptor_helper<Adaptor>;
        using ptr_helper = is_pointer<T>;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if(key.empty() && serde_type_checker<Adaptor>::is_null(ctx.adaptor)) {
                if(!data) data = ptr_helper::null_type;
            }
            else if(!key.empty() && Helper::is_null(ctx.adaptor, key)) {
                if(!data) data = ptr_helper::null_type;
            } else {
                if(!data) data = ptr_helper::init();
                serde_serializer<typename ptr_helper::type, serde_ctx>::from(ctx, *data, key);
            }
        }
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            if(data) { serde_serializer<typename ptr_helper::type, serde_ctx>::into(ctx, *data, key); }
        }
    };

    template<typename T, class Context=serde_context<detail::dummy_adaptor, true>, typename=void>
    struct to_tuple {
        using origin = meta::remove_cvref_t<T>;
        using type = T;
    };

    template<typename T>
    struct to_tuple<T, serde_context<detail::dummy_adaptor, true>, std::enable_if_t<type::is_struct_v<T>>>{
        using origin = meta::remove_cvref_t<T>;
        using Context = serde_context<detail::dummy_adaptor, true>;
        using type = typename decltype(std::declval<T>().template
                                    /*auto*/ serde<Context>(/*Context& ctx)*/
                                        std::add_lvalue_reference_t<Context>(std::declval<Context>()), /*format& */
                                        std::add_lvalue_reference_t<T>(std::declval<T>()) /*value& */))::Tuple;

    };
    /*compile time: stuct type -> tuple type*/
    template<class T> using to_tuple_t = typename to_tuple<T>::type;

    template<class T> struct tuple_size;

    template<class T> struct tuple_size :
        public std::integral_constant<std::size_t, std::tuple_size_v<serde::to_tuple_t<T>>> {};

    /*compile time: stuct member size*/
    template<class T> [[maybe_unused]] constexpr static size_t tuple_size_v = tuple_size<T>::value;
} // namespace serde


#endif

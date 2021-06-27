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
#include "serdepp/meta.hpp"
#include <nameof.hpp>
#include <magic_enum.hpp>
#include "serdepp/exception.hpp"

namespace serde
{
    using namespace serde::meta;

    template <class Adaptor> struct serde_adaptor_helper {
        // for support optional type parse
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key);
        // for support no_remain function
        inline constexpr static size_t size(Adaptor& adaptor);
        // for support string_or_struct
        inline constexpr static bool is_struct(Adaptor& adaptor);
    };

    template <class Adaptor>
    struct derive_serde_adaptor_helper { // default serde adaptor helper
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key) {
            throw serde::unimplemented_error(fmt::format("serde_adaptor<{}>::is_null(adaptor, key)",
                                                         nameof::nameof_short_type<Adaptor>()));
            return adaptor.contains(key);
        }

        inline constexpr static size_t size(Adaptor& adaptor) {
            throw serde::unimplemented_error(fmt::format("serde_adaptor<{}>::size(adaptor, key)",
                                                         nameof::nameof_short_type<Adaptor>()));
            return adaptor.size();
        }

        inline constexpr static bool is_struct(Adaptor& adaptor) {
            throw serde::unimplemented_error(fmt::format("serde_adaptor<{}>::is_struct(adaptor, key)",
                                                          nameof::nameof_short_type<Adaptor>()));
            return true;
        }
    };

    template<typename S, typename T, typename = void>
    struct serde_adaptor {
        static void from(S& s, std::string_view key, T& data);
        static void into(S& s, std::string_view key, const T& data);
    };

    namespace detail {
        struct dummy_adaptor{
            bool contains(std::string_view key) { return true; }
            size_t size() { return 1; } 
        };
    };

    template <> struct serde_adaptor_helper<detail::dummy_adaptor>
        : derive_serde_adaptor_helper<detail::dummy_adaptor> {
    };

    template<typename T, typename serde_ctx, typename = void>
    struct serde_serializer {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<Adaptor, std::remove_reference_t<T>>::from(ctx.adaptor, key, data);
            ctx.read();
        }
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {

            serde_adaptor<Adaptor, std::remove_reference_t<T>>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };

    namespace attribute {
        struct serilzier_call_attr {
            template<typename T, typename serde_ctx>
            constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key) {
                serde::serde_serializer<T, serde_ctx>::from(ctx, data, key);
            }

            template<typename T, typename serde_ctx>
            constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key) {
                serde::serde_serializer<T, serde_ctx>::into(ctx, data, key);
            }
        };
    } // namespace attribute


    template<class T, bool is_serialize_=true>
    struct serde_context {
        using Adaptor = T;
        using Helper = serde_adaptor_helper<Adaptor>;
        constexpr static bool is_serialize = is_serialize_;
        serde_context(T& format) : adaptor(format) {}
        T& adaptor;
        size_t read_count_ = 0;
        constexpr void read() { read_count_++; }
    };


    namespace type {
        struct seq_t{};
        struct map_t{};
        struct struct_t{};
        struct enum_t{
            template<class Enum>
            inline constexpr static std::string_view to_str(Enum value) {
                return magic_enum::enum_name(value);
            }
            template<class Enum>
            inline constexpr static Enum from_str(std::string_view str) {
                auto value = magic_enum::enum_cast<Enum>(str);
                if(!value.has_value()) {
                    throw enum_error(fmt::format("{}::{}", nameof::nameof_type<Enum>(), str));
                }
                return *value;
            }
        };

        template<typename T> using map_e = typename T::mapped_type;
        template<typename T> using map_k = typename T::key_type;
        template<typename T> using seq_e = typename T::value_type;
        template<typename T> using opt_e = typename T::value_type;

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

        template<typename T> using optional_element = typename is_optional<T>::element;

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

        template<class T> using is_struct = is_serdeable<serde_context<detail::dummy_adaptor>, T>;
        template<class T> inline constexpr auto is_struct_v = is_struct<T>::value;
    }

    using namespace std::string_view_literals;

    template <typename T, class Adaptor>
    constexpr inline T serialize(Adaptor&& adaptor) {
        using origin = meta::remove_cvref_t<Adaptor>;
        T target;
        serde_context<origin> ctx(adaptor);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, ""sv);
        return target;
    }

    template<typename T, class Adaptor>
    constexpr inline void serialize_to(Adaptor&& adaptor, T& target) {
        using origin = meta::remove_cvref_t<Adaptor>;
        serde_context<origin> ctx(adaptor);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, ""sv);
    }

    template<typename T, class Adaptor>
    constexpr inline T serialize_at(Adaptor&& adaptor, std::string_view key) {
        using origin = meta::remove_cvref_t<Adaptor>;

        T target;
        serde_context<origin> ctx(adaptor);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
        return target;
    }

    template<typename T, class Adaptor>
    constexpr inline void serialize_at_to(Adaptor& adaptor, T& target, std::string_view name) {
        using origin = meta::remove_cvref_t<Adaptor>;
        serde_context<origin> ctx(adaptor);
        serde_serializer<T, serde_context<origin>>::from(ctx, target, name);
    }


    template<class Adaptor, typename T>
    constexpr inline Adaptor deserialize(T&& target) {
        using origin = meta::remove_cvref_t<T>;
        Adaptor adaptor;
        serde_context<Adaptor, false> ctx(adaptor);
        serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, ""sv);
        return adaptor;
    }

    template<class Adaptor, typename T, typename U = meta::remove_cvref_t<T>>
    constexpr inline void deserialize_from(T&& target, Adaptor& adaptor) {
        using origin = meta::remove_cvref_t<T>;
        serde_context<Adaptor, false> ctx(adaptor);
        serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, ""sv);
    }

    template<class Adaptor, typename T>
    constexpr inline Adaptor deserialize_by(T&& target, std::string_view key) {
        using origin = meta::remove_cvref_t<T>;
        Adaptor adaptor;
        serde_context<Adaptor, false> ctx(adaptor);
        serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, key);
        return adaptor;
    }

    template<class Adaptor, typename T, typename U = meta::remove_cvref_t<T>>
    constexpr inline void deserialize_by_from(Adaptor& adaptor, T&& target, std::string_view key) {
        using origin = meta::remove_cvref_t<T>;
        serde_context<Adaptor, false> ctx(adaptor);
        serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, key);
    }

    template<class Context, class T>
    class serde_struct {
        using TYPE = T;
        Context& context_;
        T& value_;
        constexpr const static std::string_view type = nameof::nameof_type<T>();
    public:
        constexpr serde_struct(Context& context, T& value) : context_(context), value_(value) {}
        //template<class MEM_PTR>
        //inline constexpr serde_struct& value_or_struct(MEM_PTR&& ptr, std::string_view name) {
        //    using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
        //    if constexpr(Context::is_serialize) {
        //        if(Context::Helper::is_struct(context_.adaptor)) {
        //            serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
        //        } else {
        //            serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, "");
        //        }
        //    } else {
        //        serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
        //    }
        //    return *this;
        //}
        template<class MEM_PTR, typename Attribute, typename... Attributes>
        inline constexpr serde_struct& field(MEM_PTR&& ptr, std::string_view name,
                                             Attribute&& attribute, Attributes&&... attributes) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(Context::is_serialize) {
                attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                                                        attributes..., attribute::serilzier_call_attr{});
            } else {
                attribute.template into<rtype, Context>(context_, value_.*ptr, name,
                                                        attributes..., attribute::serilzier_call_attr{});
            }
            return *this;
        }

        template<class MEM_PTR>
        inline constexpr serde_struct& field(MEM_PTR&& ptr, std::string_view name) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(Context::is_serialize) {
                serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
            } else {
                serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
            }
            return *this;
        }
        
        inline constexpr serde_struct& no_remain() {
            if constexpr (Context::is_serialize) {
                const auto adaptor_size = Context::Helper::size(context_.adaptor);
                const auto serde_size   = context_.read_count_;
                if(adaptor_size > serde_size) {
                    throw unregisted_data_error(fmt::format(
                                                    "serde[{}] read:{} != adaptor[{}] read:{}",
                                                    type,
                                                    serde_size,
                                                    nameof::nameof_short_type<decltype(context_.adaptor)>(),
                                                    adaptor_size));
                }
            }
            return *this;
        }

    //template<class MEM_PTR, class... Ts>
    //inline constexpr serde_struct& field(MEM_PTR&& ptr, std::string_view name, attributes<Ts...>&& opt) {
    //        using sde = serde::serde_adaptor_helper<typename Context::Adaptor>;
    //        using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
    //        if constexpr(Context::is_serialize) {
    //            constexpr auto opt_check = serde::is_optional_v<rtype>;
    //            if constexpr (opt_check) {
    //                auto& val = value_.*ptr;
    //                if(sde::is_null(context_.adaptor, name)) {
    //                    if(!val) val.emplace();
    //                    serde::serde_serializer<typename rtype::value_type, Context>::from(context_, *val, name);
    //                } else {
    //                    val = std::nullopt;
    //                }
    //            }
    //            else {
    //            serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
    //            }
    //            context_.visit(opt);
    //    } else {
    //        auto& mem_ptr = std::invoke(ptr, value_);
    //        if constexpr (serde::is_optional_v<rtype>) {
    //            fmt::print("opt:");
    //            if (mem_ptr){
    //            serde::serde_serializer<typename rtype::value_type, Context>::into(context_, *(value_.*ptr), name);
    //            }
    //        } else {
    //            serde::serde_serializer<rtype, Context>::into(context_, mem_ptr, name);
    //        }
    //        context_.visit(std::move(opt));
    //    }
    //    return *this;
    //}
    };
    template<class Context, class T> serde_struct(Context&, T&) -> serde_struct<Context, T>;

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_optional_v<T>>> {
        using value_type = type::opt_e<std::remove_reference_t<T>>;
        using Adaptor = typename serde_ctx::Adaptor;
        using Helper = serde_adaptor_helper<Adaptor>;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            if(Helper::is_null(ctx.adaptor, key)) {
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
   struct serde_serializer<T, serde_ctx, std::enable_if_t<is_serdeable_v<serde_ctx, T>>> {
       using Adaptor = typename serde_ctx::Adaptor;
       constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
           if(key.empty()) {
               serde_context struct_ctx = serde_context<Adaptor,true>{ctx.adaptor};
               data.serde(struct_ctx, data);
               ctx.read();
           } else {
               serde_adaptor<Adaptor, T, type::struct_t>::from(ctx.adaptor, key, data);
               ctx.read();
           }
       }

       constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
           if(key.empty()) {
               auto struct_ctx = serde_context<Adaptor, false>(ctx.adaptor);
               data.serde(struct_ctx, const_cast<T&>(data));
               ctx.read();
           } else {
               serde_adaptor<Adaptor, T, type::struct_t>::into(ctx.adaptor, key, data);
               ctx.read();
           }
       }
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
                                                           !is_literal_v<T>  >>{
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
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::enum_t>::from(ctx.adaptor, key, data);
            ctx.read();
        }
        constexpr inline static auto into(serde_ctx& ctx, const T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::enum_t>::into(ctx.adaptor, key, data);
            ctx.read();
        }
    };
} // namespace serde
#endif

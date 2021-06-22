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
#include "serdepp/type.hpp"
#include <nameof.hpp>

namespace serde
{
    using namespace serde::meta;

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

    template<typename T, typename serde_ctx, typename = void>
    struct serde_serializer {
        using Adaptor = typename serde_ctx::Adaptor;
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<Adaptor, std::remove_reference_t<T>>::from(ctx.adaptor, key, data);
        }
        constexpr inline static auto into(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<Adaptor, std::remove_reference_t<T>>::into(ctx.adaptor, key, data);
        }
    };

    template<class T, bool is_serialize_=true>
    struct serde_context {
        using Adaptor = T;
        constexpr static bool is_serialize = is_serialize_;

        serde_context(T& format) : adaptor(format) {}
        T& adaptor;
    };

    template<class... Ts> struct attributes : Ts... { using Ts::operator()...; };
    template<class... Ts> attributes(Ts...) -> attributes<Ts...>;

    template<class Context, class T, size_t Size>
    struct serde_struct {
        using TYPE = T;
        Context& context_;
        T& value_;
        const size_t size_ = Size;
        constexpr const static std::string_view type = nameof::nameof_type<T>();
        constexpr serde_struct(Context& context, T& value) : context_(context), value_(value) {}

        template<class MEM_PTR>
        inline constexpr serde_struct& field(MEM_PTR&& ptr, std::string_view name) {
            using sde = serde::serde_adaptor_helper<typename Context::Adaptor>;
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(Context::is_serialize) {
                constexpr auto opt_check = serde::is_optional_v<rtype>;
                if constexpr (opt_check) {
                    auto& val = value_.*ptr;
                    if(sde::is_null(context_.adaptor, name)) {
                        if(!val) val.emplace();
                        serde::serde_serializer<typename rtype::value_type, Context>::from(context_, *val, name);
                    } else {
                        val = std::nullopt;
                    }
                }
                else {
                    serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
                }
            } else {
                auto& mem_ptr = std::invoke(ptr, value_);
                if constexpr (serde::is_optional_v<rtype>) {
                    if (mem_ptr){
                        serde::serde_serializer<typename rtype::value_type, Context>::into(context_,
                                                                                           *(value_.*ptr),
                                                                                           name);
                    }
                } else {
                    serde::serde_serializer<rtype, Context>::into(context_, mem_ptr, name);
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

    //template<size_t Size, class Context, class T> serde_struct<Size, Context, T>(Context&, T&) {};

    template<typename T, typename serde_ctx>
     struct serde_serializer<T, serde_ctx, std::enable_if_t<is_optional_v<T>>> {
         using value_type = typename std::remove_reference_t<T>::value_type;
         using Adaptor = typename serde_ctx::Adaptor;
         using Helper = default_serde_adaptor_helper<Adaptor>;
         constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
             fmt::print("optional=>");
             //if(Helper::is_null(ctx.adaptor, key)) {
             //    data = std::nullopt;
             //} else {
                 serde_serializer<value_type, serde_ctx>::from(ctx, *data, key);
                 // }
         }
         constexpr inline static auto into(serde_ctx& ctx, T& data, std::string_view key) {
             fmt::print("optional=>");
             if(data) {
                 serde_serializer<value_type, serde_ctx>::into(ctx, *data, key);
             }
         }
         template<typename U>
         constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key, U&& default_value) {
             //if constexpr(is_serdeable_v<serde_ctx, T>) {
             //    
             //} else {
                 if(Helper::is_null(ctx.adaptor, key)) {
                     data = default_value;
                 } else {
                     serde_serializer<value_type, serde_ctx>::from(ctx, *data, key);
                 }
                 //}
         }

         template<typename U>
         constexpr inline static auto into(serde_ctx& ctx, T& data, std::string_view key, U&& default_value) {
             if(!data) data = default_value;
             serde_serializer<value_type, serde_ctx>::into(ctx, std::forward<value_type>(*data), key);
         }
     };
  using namespace std::string_view_literals;

  template <typename T, class Adaptor>
  constexpr inline T serialize(Adaptor&& adaptor) {
    using origin = typename std::remove_reference_t<Adaptor>;
    T target;
    serde_context<origin> ctx(adaptor);
    serde_serializer<T, serde_context<origin>>::from(ctx, target, ""sv);
    return target;
  }

  template<typename T, class Adaptor>
  constexpr inline void serialize_to(Adaptor&& adaptor, T& target) {
      using origin = typename std::remove_reference_t<Adaptor>;
      serde_context<origin> ctx(adaptor);
      serde_serializer<T, serde_context<origin>>::from(ctx, target, ""sv);
  }

  template<typename T, class Adaptor>
  constexpr inline T serialize_at(Adaptor&& adaptor, std::string_view key) {
      using origin = typename std::remove_reference_t<Adaptor>;
      T target;
      serde_context<origin> ctx(adaptor);
      serde_serializer<T, serde_context<origin>>::from(ctx, target, key);
      return target;
  }

  template<typename T, class Adaptor>
  constexpr inline void serialize_at_to(Adaptor& adaptor, T& target, std::string_view name) {
      using origin = typename std::remove_reference_t<Adaptor>;
      serde_context<origin> ctx(adaptor);
      serde_serializer<T, serde_context<origin>>::from(ctx, target, name);
  }


  template<class Adaptor, typename T>
  constexpr inline Adaptor deserialize(T&& target) {
      using origin = typename std::remove_reference_t<T>;
      Adaptor adaptor;
      serde_context<Adaptor, false> ctx(adaptor);
      serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, ""sv);
      return adaptor;
  }

  template<class Adaptor, typename T>
  constexpr inline Adaptor deserialize_by(T&& target, std::string_view key) {
      using origin = typename std::remove_reference_t<T>;
      Adaptor adaptor;
      serde_context<Adaptor, false> ctx(adaptor);
      serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, key);
      return adaptor;
  }

  template<class Adaptor, typename T>
  constexpr inline Adaptor deserialize_by_from(Adaptor& adaptor, T&& target, std::string_view key) {
      using origin = typename std::remove_reference_t<T>;
      serde_context<Adaptor, false> ctx(adaptor);
      serde_serializer<origin, serde_context<Adaptor,false>>::into(ctx, target, key);
      return adaptor;
  }

   template<typename T, typename serde_ctx>
   struct serde_serializer<T, serde_ctx, std::enable_if_t<is_serdeable_v<serde_ctx, T>>> {
       using Adaptor = typename serde_ctx::Adaptor;
       constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
           if(key.empty()) {
               serde_context struct_ctx = serde_context<Adaptor,true>{ctx.adaptor};
               data.serde(struct_ctx, data);
           } else {
               serde_adaptor<Adaptor, T, type::struct_t>::from(ctx.adaptor, key, data);
           }
       }
       constexpr inline static auto into(serde_ctx& ctx, T& data, std::string_view key) {
           if(key.empty()) {
               auto struct_ctx = serde_context<Adaptor, false>(ctx.adaptor);
               data.serde(struct_ctx, data);
           } else {
               serde_adaptor<Adaptor, T, type::struct_t>::into(ctx.adaptor, key, data);
           }
       }
   };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_mappable_v<T> &&
                                                           is_emptyable_v<T> >> {
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::map_t>::from(ctx.adaptor, key, data);
        }
        constexpr inline static auto into(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::map_t>::into(ctx.adaptor, key, data);
        }
    };

    template<typename T, typename serde_ctx>
    struct serde_serializer<T, serde_ctx, std::enable_if_t<is_sequenceable_v<T> &&
                                                           is_emptyable_v<T> &&
                                                           !is_literal_v<T>  >>{
        constexpr inline static auto from(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::seq_t>::from(ctx.adaptor, key, data);
        }
        constexpr inline static auto into(serde_ctx& ctx, T& data, std::string_view key) {
            serde_adaptor<typename serde_ctx::Adaptor, T, type::seq_t>::into(ctx.adaptor, key, data);
        }
    };
} // namespace serde
#endif

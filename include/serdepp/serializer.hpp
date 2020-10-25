#pragma once

#ifndef __CPPSER_SERIALIZER_HPP__
#define __CPPSER_SERIALIZER_HPP__

#include <string>
#include <vector>
#include <list>
#include <optional>
#include <variant>
#include <iostream>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <deque>
#include <unordered_map>
#include <map>

#include "serdepp/meta.hpp"

namespace serde
{
    template<typename S, typename T>
    struct serde_adaptor {
        static auto from(S& s, const std::string& key);
        static void to(S& s, const std::string& key, T& data);
        static bool is(S &s, const std::string &key);
    };
    template< typename S> struct structure { S format; };

    //struct serailizer
    template<typename T> struct serializer {
        template<bool is_serialize, typename serde_ctx> 
        static auto serde(serde_ctx& ctx, const std::string& key, T& data) {
            using format = typename serde_ctx::format;
            using adaptor = serde_adaptor<format, structure<format>>;
            if(key == "") { data.serde(ctx); }
            else {
                ctx.back_trace_.push_back(key); 
                if constexpr (is_serialize) {
                    auto& store = ctx.con_;
                    ctx.con_ = adaptor::from(ctx.con_, key).format;
                    data.serde(ctx);
                    ctx.con_ = store;
                }
                else {
                    data.serde(ctx);
                    auto nested_ctx = structure<format>{ctx.con_};
                    adaptor::to(ctx.con_=format{}, key, nested_ctx);
                }
                ctx.back_trace_.pop_back();
            }
        }
    };

    template<typename Format, bool is_serialize=true>
    class Context {
    public:
        using format = Format;
        Format& con_;
        std::deque<std::string> back_trace_;
    public:
        Context(Format& con) :con_(con) {}
        template <typename T, typename U>
        Context& tag(std::optional<T>& target, const std::string& name, U&& default_v) {
            using sde = serde_adaptor<Format,T>;
            using ser = serializer<T>;
            target = default_v;
            if constexpr(is_serialize) {
                if (sde::is(con_, name)) { ser::template serde<is_serialize>(*this, name, target.value()); } 
            }
            else {
                ser::template serde<is_serialize>(*this, name, target.value());
            }
            return *this;
        }

        template <typename T>
        Context& tag(std::optional<T>& target, const std::string& name) {
            using sde = serde_adaptor<Format,T>;
            using ser = serializer<T>;
            if constexpr(is_serialize) {
                if (sde::is(con_, name)) {
                    T value; ser::template serde<is_serialize>(*this, name, value); target = value;
                } 
            }
            else {
                if(target) ser::template serde<is_serialize>(*this, name, target.value());
            }
            return *this;
        }

        template<typename T>
        Context& tag(T& target, const std::string& name) {
            using sde = serde_adaptor<Format,T>;
            if constexpr(is_serialize) {
                if(not sde::is(con_, name)) {
                    fmt::print(stderr, "Serde(serialize error): can't find key: {}\n   -Backtrace: {}\n",
                               name, back_trace_);
                }
            }
            serializer<T>::template serde<is_serialize>(*this, name, target);
            return *this;
        }

    };

    template<typename T, typename Format>
    static T serialize(Format& con, const std::string& key="") {
        static_assert(meta::is_iterable_v<Format>, "this serailizer is not iterable");
        T target;
        Context<Format, true> p(con);
        serializer<T>::template serde<true>(p, key, target);
        return target;
    }

    template<typename Format, typename T>
    static Format deserialize(T& target, const std::string& key="") {
        static_assert(meta::is_iterable_v<Format>, "this serailizer is not iterable");
        Format con;
        Context<Format, false> p(con);
        serializer<T>::template serde<false>(p, key, target);
        return con;
    }

    // serializer gnerator macro
#define generator_serializer(TYPE) template<> struct serializer<TYPE> { \
        using Type = TYPE;\
        template<bool is_serialize, typename serde_ctx> \
        static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>; \
            if (is_serialize) { \
                data = adaptor::from(ctx.con_, key);\
            } else { \
                adaptor::to(ctx.con_, key, data);\
            } \
        }; \
    }; 
#define generator_array_serializer(TYPE) template<typename T> struct serializer<TYPE<T>> { \
        using Type = TYPE<T>;\
        template<bool is_serialize, typename serde_ctx> \
        static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>; \
            if (is_serialize) { \
                data = adaptor::from(ctx.con_, key);\
            } else { \
                adaptor::to(ctx.con_, key, data);\
            } \
        }; \
    }; 
#define generator_map_serializer(TYPE) template<typename K, typename T> struct serializer<TYPE<K,T>> { \
        using Type = TYPE<K,T>; \
        template<bool is_serialize, typename serde_ctx> \
        static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>; \
            if (is_serialize) { \
                data = adaptor::from(ctx.con_, key);\
            } else { \
                adaptor::to(ctx.con_, key, data);\
            } \
        }; \
    }; 

    generator_serializer(int32_t)
    generator_serializer(int64_t)
    generator_serializer(std::string)
    generator_serializer(float)
    generator_serializer(double)
    generator_serializer(char)
    generator_array_serializer(std::vector)
    generator_array_serializer(std::list)
    generator_array_serializer(std::deque)
    generator_map_serializer(std::map)
    generator_map_serializer(std::unordered_map)
} // namespace cppser

#endif


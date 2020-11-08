#pragma once

#ifndef __CPPSER_SERIALIZER_HPP__
#define __CPPSER_SERIALIZER_HPP__

#include <string>
#include <vector>
#include <list>
#include <optional>
#include <iostream>
#include <deque>
#include <unordered_map>
#include <map>
#include <fmt/format.h>
#include <fmt/ranges.h>

namespace serde
{
    namespace meta {
        template <typename T, typename = void> struct is_iterable : std::false_type {};
        template <typename T>
        struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                          decltype(std::end(std::declval<T>()))>> : std::true_type {};
        template <typename T> constexpr bool is_iterable_v = is_iterable<T>::value;
        template<typename T> constexpr auto remove_const(T& type) { return static_cast<std::remove_const_t<T>>(type); }
    };


    template<typename S>
    struct serde_adaptor_helper {
        static S init() { return S{}; }
        static S parse_file(const std::string& path);
        static bool contains(S& s, const std::string& key) { return s.contains(key); }
        static int size(S& s) { return s.size(); }
    };

    template<typename S, typename T>
    struct serde_adaptor {
        static auto from(S& s, const std::string& key);
        static void   to(S& s, const std::string& key, T& data);
        static bool   is(S &s, const std::string& key);
    };

    template<typename S>
    struct base_adaptor{
        static bool is(S &s, const std::string& key) { return serde_adaptor_helper<S>::contains(s, key); }
    };

    //struct serailizer
    template<typename T> struct serializer {
        template<bool is_serialize, typename serde_ctx> 
        static auto serde(serde_ctx& ctx, const std::string& key, T& data) {
            using format = typename serde_ctx::format;
            using adaptor = serde_adaptor<format, format>;
            size_t prev_count = ctx.count_;
            ctx.count_= 0;
            if(key == "") { data.serde(ctx); }
            else {
                ctx.back_trace_.push_back(key); 
                if constexpr (is_serialize) {
                    auto& store = ctx.con_;
                    ctx.con_ = adaptor::from(ctx.con_, key);
                    data.serde(ctx);
                    ctx.con_ = store;
                }
                else {
                    data.serde(ctx);
                    auto nested_ctx = ctx.con_;
                    adaptor::to(ctx.con_= serde_adaptor_helper<format>::init(), key, nested_ctx);
                }
                ctx.back_trace_.pop_back();
            }
            ctx.count_ = prev_count;
        }
        constexpr static std::string_view type = "struct";
    };

    using namespace std::literals;
    template<typename T> inline constexpr bool is_struct() { return serializer<T>::type == "struct"sv; }
    template<typename T> inline constexpr bool is_map() { return serializer<T>::type == "map"sv; }
    template<typename T> inline constexpr bool is_array() { return serializer<T>::type == "array"sv; }

    template<class Format, bool is_serialize=true>
    class Context {
    public:
        using format = Format;
        Format& con_;
        std::deque<std::string> back_trace_;
        size_t count_;
    public:
        Context(Format& con) :con_(con) {}
        template <typename T, typename U>
        Context& tag(std::optional<T>& target, const std::string& name, U&& default_v) {
            using sde = serde_adaptor<Format,T>;
            using ser = serializer<T>;
            target = default_v;
            if constexpr(is_serialize) {
                if (sde::is(con_, name)) {
                    count_++;
                    ser::template serde<is_serialize>(*this, name, target.value());
                } 
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
                    count_++;
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
                  fmt::print(stderr, "Serde(serialize error): can't find key: {}\n   - " "Backtrace: {}\n",
                             name, back_trace_);
                }
                count_++;
            }
            serializer<T>::template serde<is_serialize>(*this, name, target);
            return *this;
        }

        void no_remain() {
            if constexpr (is_serialize) {
                fmt::print("count:{} == size:{}\n",count_, serde_adaptor_helper<Format>::size(con_));
                if(count_ < serde_adaptor_helper<Format>::size(con_)) {
                    fmt::print(stderr, "Serde(serialize error): unregisted data:\n   - " "Backtrace: {}\n", back_trace_);
                }
            }
        }
    };

    template <typename T, typename Adaptor>
    T serialize(Adaptor& con, const std::string& key = "") {
        T target;
        Context<Adaptor, true> p(con);
        serializer<T>::template serde<true>(p, key, target);
        return target;
    }

    template<typename Adaptor, typename T>
    Adaptor deserialize(T& target, const std::string& key="") {
        auto origin = meta::remove_const<T>(target);
        Adaptor con = serde_adaptor_helper<Adaptor>::init();
        Context<Adaptor, false> p(con);
        serializer<decltype(origin)>::template serde<false>(p, key, origin);
        return con;
    }

    // serializer gnerator macro
#define generator_serializer(TYPE) template<> struct serializer<TYPE> { \
        using Type = TYPE;\
        template<bool is_serialize, typename serde_ctx> \
        static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>; \
            if constexpr (is_serialize) { \
                data = adaptor::from(ctx.con_, key);\
            } else { \
                adaptor::to(ctx.con_, key, data);\
            } \
        }; \
        constexpr static std::string_view type = "normal"; \
    }; 

#define generator_array_serializer(TYPE) template<typename T> struct serializer<TYPE<T>> { \
        using Type = TYPE<T>;\
        template<bool is_serialize, typename serde_ctx> \
        static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>; \
            if constexpr (is_serialize) { \
                data = adaptor::from(ctx.con_, key);\
            } else { \
                adaptor::to(ctx.con_, key, data);\
            } \
        }; \
        constexpr static std::string_view type = "array"; \
    }; 

#define generator_map_serializer(TYPE) template<typename K, typename T> struct serializer<TYPE<K,T>> { \
        using Type = TYPE<K,T>; \
        template<bool is_serialize, typename serde_ctx> \
        static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>; \
            if constexpr (is_serialize) { \
                data = adaptor::from(ctx.con_, key);\
            } else { \
                adaptor::to(ctx.con_, key, data);\
            } \
        }; \
        constexpr static std::string_view type = "map"; \
    }; 

    generator_serializer(int32_t)
    generator_serializer(int64_t)
    generator_serializer(std::string)
    generator_serializer(std::string_view)
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

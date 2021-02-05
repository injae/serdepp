#pragma once

#ifndef __CPPSER_SERIALIZER_HPP__
#define __CPPSER_SERIALIZER_HPP__

#include <string>
#include <string_view>

#include <vector>
#include <list>
#include <optional>
#include <iostream>
#include <deque>
#include <unordered_map>
#include <map>

#include "serdepp/meta.hpp"
#include <iterator>

namespace serde
{
    struct serde_exception : std::exception {
        serde_exception(const std::string& msg) : message(msg) {};
        std::string message;
        const char* what() const throw() { return message.c_str(); }
    };

    template<typename S>
    struct serde_adaptor_helper {
        static S init() { return S{}; }
        static S parse_file(const std::string& path);
        static bool contains(S& s, const std::string& key) { return s.contains(key); }
        static int size(S& s) { return s.size(); }
        static bool is_struct(S& s) { return true; };
    };

    template<typename S>
    struct default_serde_adaptor_helper {
        static S init() { return S{}; }
        static S parse_file(const std::string& path);
        static bool contains(S& s, const std::string& key) { return s.contains(key); }
        static int size(S& s) { return s.size(); }
        static bool is_struct(S& s) { return true; };
    };


    template<typename S, typename T, typename = void>
    struct serde_adaptor {
        static void from(S& s, const std::string& key, T& data);
        static void into(S& s, const std::string& key, T& data);
    };

    template<typename S>
    struct base_adaptor{
        static bool is(S &s, const std::string& key) { return serde_adaptor_helper<S>::contains(s, key); }
    };

    namespace type {
        struct map{};
        struct seq{};
        struct struct_t{};
    };

    //struct serailizer
    template<typename T, typename = void> struct serializer {
        template<bool is_serialize, typename serde_ctx> 
        constexpr static auto serde(serde_ctx& ctx, const std::string& key, T& data) {
            using format = typename serde_ctx::format;
            using adaptor = serde_adaptor<format, T, type::struct_t>;
            size_t prev_count = ctx.count_;
            ctx.count_= 0;
            if(key == "") { data.serde(ctx); }
            else {
                if constexpr (is_serialize) { adaptor::from(ctx.con_, key, data); }
                else                        { adaptor::into(ctx.con_, key, data); }
            }
            ctx.count_ = prev_count;
        }
        constexpr static std::string_view type = "struct";
    };

    using namespace std::literals;

    template<typename T> inline constexpr bool is_struct() { return serializer<T>::type == "struct"sv; }
    template<typename T> inline constexpr bool is_map()    { return serializer<T>::type == "map"sv; }
    template<typename T> inline constexpr bool is_array()  { return serializer<T>::type == "array"sv; }

    template<class Format, bool is_serialize=true>
    class Context {
    public:
        using format = Format;
        constexpr static bool serialize_step = is_serialize;
        Format& con_;
        size_t count_;
        std::string_view name_;
    public:
        Context(Format& con, const std::string& name="") :con_(con), name_(name) {}

        // this function only work in serialize step
        //                                   key     struct
        // you can use this function, map<std::string, t>, t.${target} = key;
        Context& name(std::string& target) {
            if constexpr (is_serialize) { target = name_; }
            return *this;
        }

        template <typename t>
        Context& or_value(t& target, const std::string& name) {
            using sde = serde_adaptor_helper<format>;
            using ser = serializer<t>;
            if constexpr (is_serialize) {
                count_++;
                if(sde::is_struct(con_)) { 
                    ser::template serde<is_serialize>(*this, name, target);
                } else {
                    ser::template serde<is_serialize>(*this, "", target);
                }
            } else {
                tag(target, name);
            }
            return *this;
        }
        template <typename t, typename u>
        Context& or_value(std::optional<t>& target, const std::string& name, u&& default_v) {
            using sde = serde_adaptor_helper<format>;
            using ser = serializer<t>;
            if(not target) target = default_v;
            if constexpr (is_serialize) {
                count_++;
                if(sde::is_struct(con_)) { 
                    ser::template serde<is_serialize>(*this, name, *target);
                } else {
                    ser::template serde<is_serialize>(*this, "", *target);
                }
            } else {
                tag(target, name, default_v);
            }
            return *this;
        }

        template <typename T, typename U>
        Context& tag(std::optional<T>& target, const std::string& name, U&& default_v) {
            using sde = serde_adaptor_helper<Format>;
            using ser = serializer<T>;
            bool is_default = false;
            if(not target) {target = default_v; is_default = true; }
            if constexpr(is_serialize) {
                if(sde::is_struct(con_) && sde::contains(con_, name)) {
                    count_++;
                    ser::template serde<is_serialize>(*this, name, *target);
                } 
                else if(is_struct<T>() && is_default) {
                    count_++;
                    auto temp_format = sde::init();
                    auto temp_ctx = Context(temp_format);
                    ser::template serde<is_serialize>(temp_ctx, "", *target);
                }
            }
            else {
                ser::template serde<is_serialize>(*this, name, *target);
            }
            return *this;
        }

        template <typename T>
        Context& tag(std::optional<T>& target, const std::string& name) {
            using sde = serde_adaptor_helper<Format>;
            using ser = serializer<T>;
            if constexpr(is_serialize) {
                if(sde::is_struct(con_) && sde::contains(con_, name)) {
                    count_++;
                    target = T{}; ser::template serde<is_serialize>(*this, name, *target); 
                } else {
                    target = std::nullopt;
                }
            }
            else {
                if(target) ser::template serde<is_serialize>(*this, name, *target);
            }
            return *this;
        }

        template<typename T>
        Context& tag(T& target, const std::string& name) {
            using sde = serde_adaptor_helper<Format>;
            if constexpr(is_serialize) {
                if(sde::is_struct(con_) && not sde::contains(con_, name) ) {
                    throw serde_exception("Serde(serialize error): can't find key: "+ name + '\n');
                }
                count_++;
            }
            serializer<T>::template serde<is_serialize>(*this, name, target);
            return *this;
        }

        void no_remain() {
            if constexpr (is_serialize) {
                if(count_ < serde_adaptor_helper<Format>::size(con_)) {
                  throw serde_exception{
                      "Serde(serialize error): unregisted data:"s +
                      std::to_string(count_) + " " +
                      std::to_string(serde_adaptor_helper<Format>::size(con_)) +
                      std::string{name_} + "\n"};
                }
            }
        }
    };
    template <typename T, typename Adaptor>
    T serialize_element(Adaptor& con, const std::string& key) {
        T target;
        Context<Adaptor, true> p(con, key);
        serializer<T>::template serde<true>(p, key, target);
        return target;
    }

    template <typename T, typename Adaptor>
    T serialize(Adaptor& con, const std::string& name = "") {
        T target;
        Context<Adaptor, true> p(con, name);
        serializer<std::remove_reference_t<T>>::template serde<true>(p, "", target);
        return target;
    }

    template <typename T, typename Adaptor>
    void serialize_to(Adaptor& con, T& target, const std::string& name = "") {
        Context<Adaptor, true> p(con, name);
        serializer<std::remove_reference_t<T>>::template serde<true>(p, "", target);
    }

    template <typename T, typename Adaptor>
    void serialize_to_element(Adaptor& con, T& target, const std::string& key) {
        Context<Adaptor, true> p(con, key);
        serializer<std::remove_reference_t<T>>::template serde<true>(p, key, target);
    }

    template<typename Adaptor, typename T>
    Adaptor deserialize(T& target, const std::string& name="") {
        auto origin = meta::remove_const<T>(target);
        Adaptor con = serde_adaptor_helper<Adaptor>::init();
        Context<Adaptor, false> p(con, name);
        serializer<decltype(origin)>::template serde<false>(p, "", origin);
        return con;
    }

    template<typename Adaptor, typename T>
    Adaptor deserialize_with_name(T& target, const std::string& name) {
        auto origin = meta::remove_const<T>(target);
        Adaptor con = serde_adaptor_helper<Adaptor>::init();
        Context<Adaptor, false> p(con, name);
        serializer<decltype(origin)>::template serde<false>(p, name, origin);
        return con;
    }

    template<typename S>
    inline S parse_file(const std::string& path) { return serde_adaptor_helper<S>::parse_file(path); }

    template<typename S, typename T>
    inline T parse_file_and_serde(const std::string& path) {
        auto con = serde_adaptor_helper<S>::parse_file(path);
        return serialize<T>(con);
    }

#define regist_serde_sequence_type(TYPE) template<class T> struct meta::is_sequence<TYPE<T>> { \
        using type = TYPE<T>;\
        using element = T;\
        constexpr static bool value = true;\
    };

#define regist_serde_map_type(TYPE) template<class T, class U> struct meta::is_map<TYPE<T,U>> { \
        using type = TYPE<T,U>;\
        using key = T;\
        using element = U;\
        constexpr static bool value = true;\
    };

    // serializer gnerator macro
#define generator_serializer(TYPE) template<> struct serializer<TYPE> { \
        using Type = TYPE;\
        template<bool is_serialize, typename serde_ctx> \
        constexpr static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type>;\
            if constexpr (is_serialize) {  adaptor::from(ctx.con_, key, data); } \
            else                        {  adaptor::into(ctx.con_, key, data); } \
        }; \
        constexpr static std::string_view type = "normal"; \
    }; 

#define generator_sequence_serializer(TYPE) template<typename T> struct serializer<TYPE<T>> { \
        using Type = TYPE<T>;\
        template<bool is_serialize, typename serde_ctx> \
        constexpr static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type, type::seq>; \
            if constexpr (is_serialize) { adaptor::from(ctx.con_, key, data);} \
            else                        { adaptor::into(ctx.con_, key, data);  } \
        }; \
        constexpr static std::string_view type = "sequence"; \
    }; \
    regist_serde_sequence_type(TYPE)

#define generator_map_serializer(TYPE) template<typename K, typename T> struct serializer<TYPE<K,T>> { \
        using Type = TYPE<K,T>; \
        template<bool is_serialize, typename serde_ctx> \
        constexpr static auto serde(serde_ctx& ctx, const std::string& key, Type& data) {\
            using adaptor = serde_adaptor<typename serde_ctx::format, Type, type::map>; \
            if constexpr (is_serialize) { adaptor::from(ctx.con_, key, data); } \
            else                        { adaptor::into(ctx.con_, key, data);   } \
        }; \
        constexpr static std::string_view type = "map"; \
    }; \
    regist_serde_map_type(TYPE)

    // undefined type convert other type
    template<class FROM, class INTO>
    struct serializer_convertor {
        using TYPE = FROM;
        template<bool is_serialize, typename serde_ctx> 
        constexpr static auto serde(serde_ctx& ctx, const std::string& key, FROM& data) {
            using adaptor = serde_adaptor<typename serde_ctx::format, INTO>;
            INTO convert_type;
            if constexpr (is_serialize) {
                adaptor::from(ctx.con_, key, convert_type);
                serializer<FROM>::from(data, convert_type);
            } 
            else {
                serializer<FROM>::into(convert_type, data);
                adaptor::into(ctx.con_, key, convert_type);
            } 
        }; 
        constexpr static std::string_view type = "normal"; 
    };

    generator_serializer(bool)
    generator_serializer(int16_t)
    generator_serializer(int32_t)
    generator_serializer(int64_t)
    generator_serializer(std::string)
    generator_serializer(std::string_view)

    generator_serializer(double)
    generator_serializer(char)
    generator_sequence_serializer(std::vector)
    generator_sequence_serializer(std::deque)
    generator_sequence_serializer(std::list)
    generator_map_serializer(std::map)
    generator_map_serializer(std::unordered_map)
} // namespace serde
#endif

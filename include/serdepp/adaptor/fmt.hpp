#pragma once

#ifndef __SERDEPP_ADAPTOR_FMT_HPP__
#define __SERDEPP_ADAPTOR_FMT_HPP__

#include <serdepp/serializer.hpp>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/format.h>

namespace serde {
    using namespace fmt::literals;
    class literal {
    public:
        literal() {}
        literal(const std::string& str) { data_ = str; }
        literal(const literal& other) :  members_(other.members_), data_(other.data_) {}
        inline void add_child(const std::string& key, literal child) {
            child.data_ = "{}:"_format(key);
            members_.push_back(child);
        }
        template<typename T>
        inline void add_child(const std::string& key, const T& child) {
            members_.push_back({"{}:{}"_format(key, child)});
        }

        std::string to_string(int step=0) {
            if(!members_.empty()) {
                if(step != 0) data_ += "{\n";
                for(auto& it : members_) {
                    auto str = it.to_string(step + 1);
                    data_ += "{:>{}}\n"_format(str, step * 4 + str.size());
                }
                if(step != 0) data_ += "{:>{}}"_format("}", (step * 4)-4);
            }
            return data_;
        }
        bool contains(const std::string& key)  { return true; }
    private:
        std::list<literal> members_;
        std::string data_;
    };

    template<typename T> struct serde_adaptor<literal, T, type::struct_t> {
        static void from(literal& s, const std::string& key, T& data) { /*unimplemented*/ }
        static void   to(literal &s, const std::string &key, T& data) { s.add_child(key, deserialize<literal>(data)); }
    };

    template<typename T, typename U> struct serde_adaptor<literal, T, U> {
        static void from(literal& s, const std::string& key, T& data){ /*unimplemented*/ }
        static void to(literal& s, const std::string& key, T& data) { s.add_child(key,data); }
        static void from(literal& s, T& data){ /*unimplemented*/ }
        static void to(literal& s, T& data) { s.add_child("", data); }
    };
}

#endif

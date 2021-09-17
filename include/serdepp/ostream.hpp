#pragma once

#ifndef __SERDEPP_OSTREAM_HPP__
#define __SERDEPP_OSTREAM_HPP__

#include "serdepp/adaptor/sstream.hpp"

namespace serde::ostream {
    //template<class Ostream, class T>
    //Ostream&& operator<<(Ostream&& os, const T& x) {
    //    if constexpr (type::is_struct_v<T>) {
    //        return os << serde::deserialize<serde::literal>(x).to_string();
    //    } else {
    //        return os << x;
    //    }
    //}
    template< class CharT, class Traits, typename T>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT,Traits>& os, const T& x) {
        if constexpr (type::is_struct_v<T>) {
            return os << serialize<serde_sstream>(x).str();
        } else {
            return os << x;
        }
    }
};

#endif

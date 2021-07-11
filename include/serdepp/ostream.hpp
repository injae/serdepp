#pragma once

#ifndef __SERDEPP_OSTREAM_HPP__
#define __SERDEPP_OSTREAM_HPP__

#include "serdepp/adaptor/fmt.hpp"

namespace serde::ostream {
    template<class Ostream, class T>
    Ostream&& operator<<(Ostream&& os, const T& x) {
        if constexpr (type::is_struct_v<meta::remove_cvref<T>>) {
            return os << serde::deserialize<serde::literal>(x).to_string();
        } else {
            return os << x;
        }
    }
};

#endif

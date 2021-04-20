#pragma once

#ifndef __SERDEPP_OSTREAM_HPP__
#define __SERDEPP_OSTREAM_HPP__

#include "serdepp/adaptor/fmt.hpp"

namespace serde::ostream {
    template<class Ostream, class T>
    Ostream&& operator<<(Ostream&& os, const T& x) {
        if constexpr (is_serdeable_v<serde::to_string,T>) { return os << "ostrem\n"; }
        return os << x;
    }
};

#endif

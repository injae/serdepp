#pragma once

#ifndef __SERDEPP_ATTRIBUTE_META_HPP__
#define __SERDEPP_ATTRIBUTE_META_HPP__

#include "serdepp/serializer.hpp"

namespace serde::attribute{

    template<class Next>
    constexpr static auto is_serializer_call_v = std::is_same_v<meta::remove_cvref_t<Next>,
                                                                detail::serializer_call_attr>;

}



#endif

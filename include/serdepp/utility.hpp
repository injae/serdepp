#pragma once

#include "serdepp/serializer.hpp"
#include "serdepp/adaptor/fmt.hpp"

#ifndef __SERDE_UTILITY_HPP__
#define __SERDE_UTILITY_HPP__

#ifdef __SERDEPP_ADAPTOR_FMT_HPP__
    #define derive_serde(TYPE, ...) friend serde::serializer<TYPE>; \
        template<typename Ctx> auto serde(Ctx& ctx) { \
            __VA_ARGS__ \
        } \
        friend std::ostream& operator<<(std::ostream& os, const TYPE& type) { \
        return os << serde::deserialize_with_name<serde::literal>(type, #TYPE).to_string(); \
        }                                           
#else
    #define derive_serde(TYPE, ...) friend serde::serializer<TYPE>; \
        template<typename Ctx> auto serde(Ctx& ctx) { \
            __VA_ARGS__ \
        } 
#endif

#define TAG(var, ...) tag(var, #var, __VA_ARGS__)

#endif

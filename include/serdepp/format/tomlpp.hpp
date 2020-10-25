#pragma once

#ifndef __SERDE_FORMAT_TOMLPP_HPP__
#define __SERDE_FORMAT_TOMLPP_HPP__

#include "tomlpp/toml.hpp"
#include "serdepp/serializer.hpp"

namespace serde {
    using toml = std::shared_ptr<toml::table>;
    template<typename T> struct serde_adaptor<toml,T> {
        static auto from(toml& s, const std::string& key) {return s->template get_as<T>(key).value(); }
        static void to(toml& s, const std::string& key, T& data) { s->insert(key, data); }
        static bool is(toml& s, const std::string& key) { return s->contains(key); }
    };
}

#endif

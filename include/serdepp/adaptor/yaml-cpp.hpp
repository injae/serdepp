#pragma once

#ifndef __SERDEPP_ADAPTOR_YAML_CPP_HPP__
#define __SERDEPP_ADAPTOR_YAML_CPP_HPP__

#include <yaml-cpp/yaml.h>
#include "serdepp/serializer.hpp"

namespace serde {
    using yaml = YAML::Node;
}


#endif

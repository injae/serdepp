

#include "define.h"
#include <serdepp/adaptor/yaml-cpp.hpp>

int main(int argc, char* argv[]) {
    YAML::Node node = YAML::Load(R"(ttt:
  bbb:
    str: ssss
    v:
    - h
    - e
    - l
    - l
    - o
    m:
      a: b
      c: d
)");
    fmt::print("{}\n",node);
    auto xx = serde::serialize_element<ttt>(node, "ttt");
    fmt::print("{}\n",xx);

    auto yy = serde::deserialize_with_name<YAML::Node>(xx, "ttt");
    fmt::print("{}\n",yy);
    auto zzz = serde::serialize_element<ttt>(yy, "ttt");
    fmt::print("{}\n",zzz);

    return 0;
}


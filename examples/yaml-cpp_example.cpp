

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
    auto xx = serde::serialize<ttt>(node, "ttt");
    fmt::print("{}\n",xx);

    auto yy = serde::deserialize<YAML::Node>(xx, "ttt");
    fmt::print("{}\n",yy.dump());
    auto zzz = serde::serialize<ttt>(yy, "ttt");
    fmt::print("{}\n",zzz);

    return 0;
}


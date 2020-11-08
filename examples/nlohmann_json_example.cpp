#include "define.h"
#include "serdepp/adaptor/nlohmann_json.hpp"

int main(int argc, char* argv[]) {
    nlohmann::json cosn = R"({"ttt":{"bbb":{"str":"ssss","v": ["h","e","l", "l", "o"],"m":{"a":"b","c":"d"}}}})"_json; 
    fmt::print("{}\n",cosn.dump());
    auto xx = serde::serialize<ttt>(cosn,"ttt");
    fmt::print("{}\n",xx);

    auto yy = serde::deserialize<nlohmann::json>(xx, "ttt");
    fmt::print("{}\n",yy.dump());
    auto zzz = serde::serialize<ttt>(yy, "ttt");
    fmt::print("{}\n",zzz);

    return 0;
}

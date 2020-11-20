#include "define.h"
#include "serdepp/adaptor/nlohmann_json.hpp"
#include <variant>

int main(int argc, char* argv[]) {
    nlohmann::json with_name = R"({"ttt":{"bbb":{"str":"ssss","v": ["h","e","l", "l", "o"],"m":{"a":"b","c":"d"}}}})"_json; 
    nlohmann::json no_name = R"({"bbb":{"str":"ssss","v": ["h","e","l", "l", "o"],"m":{"a":"b","c":"d"}}})"_json; 
    fmt::print("{}\n",with_name.dump());
    auto xx_1 = serde::serialize_element<ttt>(with_name,"ttt");
    auto xx_2 = serde::serialize<ttt>(no_name, "ttt");
    fmt::print("{}\n",xx_1);
    fmt::print("{}\n",xx_2);

    auto yy_1 = serde::deserialize_with_name<nlohmann::json>(xx_1, "ttt");
    auto yy_2 = serde::deserialize<nlohmann::json>(xx_1, "ttt");
    fmt::print("{}\n",yy_1.dump());
    fmt::print("{}\n",yy_2.dump());
    auto zzz = serde::serialize_element<ttt>(yy_1, "ttt");
    fmt::print("{}\n",zzz);

    return 0;
}

#include "serdepp/adaptor/tomlpp.hpp"
#include "define.h"

int main(int argc, char* argv[]) {
    std::string data = R"(
[bbb]
str = "ssss"
v = [ "h", "e", "l", "l", "o" ]
z= "10"

    [bbb.m]
    a = "b"
    c = "d"
)"; 
    std::istringstream stream(data);
    auto t = toml::parser(stream).parse();

    auto xx = serde::serialize<ttt>(t);
    fmt::print("{}\n",xx);

    auto yy = serde::deserialize<toml_ptr>(xx, "ttt");
    std::cout << (*yy) << std::endl;


    return 0;
}

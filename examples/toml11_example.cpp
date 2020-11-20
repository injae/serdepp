#include "define.h"
#include "serdepp/adaptor/toml11.hpp"

int main(int argc, char* argv[]) {
    std::string data = R"(
[ttt]
    [ttt.inmap]
        x.text="hello"
        y.text="world"
    [ttt.bbb]
        str = "ssss"
        v = [ "h", "e", "l", "l", "o" ]
        m = {a="b", c="d"}
)"; 
    std::istringstream stream(data);
    auto t = toml::parse(stream);
    fmt::print("{}\n",t);
    auto xx = serde::serialize_element<ttt>(t, "ttt");
    fmt::print("{}\n",xx);

    auto yy = serde::deserialize_with_name<toml::value>(xx, "ttt");
    std::cout << yy << std::endl;

    return 0;
}


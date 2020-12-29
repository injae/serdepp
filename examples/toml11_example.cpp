#include "define.h"
#include "serdepp/adaptor/toml11.hpp"

struct test {
    derive_serde(test, ctx
                 .name(name)
                 .or_value(z, "z")
                 .TAG_OR(y, "hello");
                 )
    std::string z;
    std::string name;
    std::optional<std::string> y;
};

struct p_test {
    derive_serde(p_test, ctx
                 .TAG(m)
                 .TAG(n);
                 )
    std::map<std::string, test> m;
    std::map<std::string, std::string> n;
};

int main(int argc, char* argv[]) {

  {
    std::string t_data = R"(
      [m]
        x = "1"
        y = { z = "h", y = "hi"}
      [n]
        x = "1"
        y = "1"
    )";
    std::istringstream t_stream(t_data);
    auto t_t = toml::parse(t_stream);
    auto t_xx = serde::serialize<p_test>(t_t, "p_test");
    fmt::print("{}\n",t_xx);
    auto t_yy = serde::deserialize<toml::value>(t_xx);
    fmt::print("{}\n",t_yy);
  }
  

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


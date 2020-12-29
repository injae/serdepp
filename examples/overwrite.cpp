#include "serdepp/utility.hpp"
#include "serdepp/adaptor/toml11.hpp"

struct test {
    derive_serde(test, ctx
                 .name(name)
                 .or_value(z, "z")
                 .tag(y, "y", "hello")
                 .tag(t, "t");
                 )
    std::string z;
    std::string name;
    std::optional<std::string> y;
    std::optional<std::string> t;
};

struct hhh {
    derive_serde(hhh, ctx.TAG_OR(x, "hh");)
    std::optional<std::string> x;
};

struct p_test {
    derive_serde(p_test, ctx
                .TAG(m)
                .TAG_OR(hh, hhh())
                .TAG(z);)
    std::optional<std::map<std::string, test>> m;
    std::vector<test> z;
    std::optional<hhh> hh;
};

int main(int argc, char* argv[]) {
    std::string t_data = R"(
      [m]
        x = "1"
        h = { z = "h", y = "hi"}

      [[z]]
        z = "not" 

      [[z]]
        z = "why" 
    )";
    std::istringstream t_stream(t_data);
    auto t_t = toml::parse(t_stream);
    fmt::print("{}\n",t_t);
    std::optional<p_test> pt = serde::serialize<p_test>(t_t);
    
    auto o_data = R"(
        [x]
         z = "h" 
         y = "changed"
         t = "hell"

        [y]
         z = "h" 
         y = "hi"
         t = "jjj"
    )";
    std::istringstream o_stream(o_data);
    auto o_t = toml::parse(o_stream);
    serde::serialize_to(o_t, pt->m.value());
    fmt::print("{}\n",serde::deserialize<toml::value>(*pt));


    fmt::print("{}, {}\n",pt->hh.has_value(), pt->hh->x.has_value());

    return 0;
}




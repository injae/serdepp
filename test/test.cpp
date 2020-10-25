
#include "serdepp/serializer.hpp"
#include <type_traits>
#include "serdepp/format/nlohmann_json.hpp"
#include "serdepp/format/tomlpp.hpp"
#include <map>

struct bbb {
    friend serde::serializer<bbb>;
    template<typename S> auto serde(S& s) {
        s.tag(str, "str")
         .tag(x, "x", 0)
         .tag(m, "m")
         .tag(v, "v");
    }
public:
    std::string str;
    std::optional<int> x;
    std::vector<std::string> v;
    std::map<std::string, std::string> m;
};

struct ttt {
    friend serde::serializer<ttt>;
    template<typename S> auto serde(S& s) {
        s.tag(bbb, "bbb");
    }
public:
    std::string str;
    bbb bbb;
    std::vector<std::string> v;
};

int main(int argc, char* argv[]) {
    auto s = std::string{"hello"};
    nlohmann::json cosn = R"({"bbb" : { "str" : "ssss", "v": ["h","e","l", "l", "o"], "m" : {"a" : "b", "c": "d"} }})"_json; 
    //    fmt::print("{}\n",cosn.dump());
    auto xx = serde::serialize<ttt>(cosn);
    fmt::print("{}\n",xx.bbb.v);
    auto yy = serde::deserialize<nlohmann::json>(xx, "ttt");
    fmt::print("{}\n",yy.dump());


    //nlohmann::json x;
    //auto j = serde::Parser<nlohmann::json, false>{x};
    //j.el(s, "string")
    // .el(i, "int");
    //fmt::print("{}\n", x.dump());

    //fmt::print("string:{}, int:{}\n",s,i);
    //std::vector<std::string> v = {"hello", "bye"};
    //auto c = toml::make_table();
    ////c->insert("string", "4444");
    //c->insert("int", 1234);
    //serde::Parser<std::shared_ptr<toml::table>> t{c};

    //t// .el(s, "string");
    //     .el(i, "int");

    //fmt::print("string:{}, int:{}\n",s,i);


    return 0;
}

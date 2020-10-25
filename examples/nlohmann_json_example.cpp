#include "serdepp/serializer.hpp"
#include <type_traits>
#include "serdepp/format/nlohmann_json.hpp"
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
    nlohmann::json cosn = R"({"bbb":{"str":"ssss","v": ["h","e","l", "l", "o"],"m":{"a":"b","c":"d"} }})"_json; 

    auto xx = serde::serialize<ttt>(cosn);
    fmt::print("{}\n",xx.bbb.v);

    auto yy = serde::deserialize<nlohmann::json>(xx, "ttt");
    fmt::print("{}\n",yy.dump());


    return 0;
}

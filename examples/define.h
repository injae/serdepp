#include "serdepp/serializer.hpp"
#include "serdepp/adaptor/fmt.hpp"
#include "serdepp/utility.hpp"

struct in {
    derive_serde(in, ctx.tag(text, "text");)
    std::string text;
};


struct bbb {
    friend serde::serializer<bbb>;
    derive_serde(bbb,
              ctx.tag(str, "str")
                 .tag(x, "x", 10)
                 .tag(m, "m")
                 .tag(om, "om")
                 .tag(v, "v")
                 .no_remain();)
    //template<typename S> auto serde(S& s) {
    //    s.tag(str, "str")
    //     .tag(x, "x", 10)
    //     .tag(m, "m")
    //     .tag(om, "om")
    //     .tag(v, "v")
    //     .no_remain();
    //}
    //   friend std::ostream& operator<<(std::ostream& os, const bbb& type) {
    //       using namespace serde;
    //       return os << deserialize<literal>(type, "bbb").to_string();
    //   }
public:
    std::string str;
    std::optional<int> x ; //optional argument
    std::vector<std::string> v;
    std::unordered_map<std::string, std::string> m;
    std::optional<std::unordered_map<std::string, std::string>> om;
private:
    std::string z;
};

struct ttt {
    derive_serde(ttt, // macro version
        ctx.tag(bbb, "bbb")
           .tag(inmap, "inmap")
                 //.tag(x, "x")
           .tag(str, "str");
    )
    //friend serde::serializer<ttt>;
    //template<typename S> auto serde(S& s) {
    //    s.tag(bbb, "bbb"); // nested  struct
    //}
    //friend std::ostream& operator<<(std::ostream& os, ttt& type) {
    //    return os << serde::deserialize<serde::string_convertor>(type, "ttt").to_string();
    //}
public:
    int x;
    std::optional<std::string> str;
    bbb bbb;
    std::vector<std::string> v;
    std::unordered_map<std::string,in> inmap;
private:
};

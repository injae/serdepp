#include "serdepp/serializer.hpp"
#include "serdepp/adaptor/fmt.hpp"
#include "serdepp/utility.hpp"

struct bbb {
    friend serde::serializer<bbb>;
    template<typename S> auto serde(S& s) {
        s.tag(str, "str")
         .tag(x, "x", 0)
         .tag(m, "m")
         .tag(om, "om")
         .tag(v, "v")
         .no_remain();
    }
    friend std::ostream& operator<<(std::ostream& os, const bbb& type) {
        using namespace serde;
        return os << deserialize<literal>(type, "bbb").to_string();
    }
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
    // macro version
    derive_serde(ttt,
                 ctx.tag(bbb, "bbb")
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
    std::string str;
    bbb bbb;
    std::vector<std::string> v;
private:
};

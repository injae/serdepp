
#include "serdepp/serializer.hpp"
#include "serdepp/adaptor/fmt.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"

//struct bbb {
//    friend serde::serializer<bbb>;
//    template<typename S> auto serde(S& s) {
//        s.tag(str, "str")
//         .tag(x, "x", 0)
//         .tag(m, "m")
//         .tag(v, "v");
//    }
//public:
//    std::string str;
//    std::optional<int> x;
//    std::vector<std::string> v;
//    std::map<std::string, std::string> m;
//};
//
//struct ttt {
//    friend serde::serializer<ttt>;
//    template<typename S> auto serde(S& s) {
//        s.tag(bbb, "bbb");
//    }
//public:
//    std::string str;
//    bbb bbb;
//    std::vector<std::string> v;
//};

class ADT{};


struct test {
    template<class CTX> auto serde(CTX& ctx) {}
    int a;
};

using namespace serde;

int main(int argc, char* argv[]) {
    std::cout<<"tmp test"<<std::endl;
    //static_assert(is_iterable_v<std::vector<int>>);
    //static_assert(is_iterable_v<std::list<int>>);
    //static_assert(is_iterable_v<std::map<int,int>>);
    //static_assert(is_iterable_v<std::set<int>>);
    //static_assert(is_serdeable_v<serde_context<serde::to_string>, test>);
    //static_assert(is_optionable_v<std::optional<int>>);
    //static_assert(is_optionable_v<std::vector<int>>);
    //static_assert(is_optionable_v<std::map<int,int>>);
    //static_assert(is_mappable_v<std::map<int,int>>);
    //static_assert(is_mappable_v<std::unordered_map<int,int>>);
    nlohmann::json json;

    test target;
    serialize<test>(json);
    deserialize<nlohmann::json>(target);
    
    int normal;
    serialize_at<int>(R"({"num":10})"_json, "num");
    deserialize<nlohmann::json>(normal);

    std::optional<test> opt_target;
    serialize<std::optional<test>>(nlohmann::json{});
    deserialize<nlohmann::json>(opt_target);

    std::vector<test> seq_target;
    serialize<std::vector<test>>(nlohmann::json{});
    deserialize<nlohmann::json>(seq_target);

    std::map<std::string, test> map_target;
    serialize<std::map<std::string, test>>(nlohmann::json{});
    deserialize<nlohmann::json>(map_target);

    return 0;
}

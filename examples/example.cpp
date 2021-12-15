#include <serdepp/serde.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>
#include "serdepp/adaptor/rapidjson.hpp"
#include <memory>

enum class tenum {INPUT, OUTPUT, INPUT_2 , OUTPUT_2 };


struct nested {
    DERIVE_SERDE(nested,
                 (&Self::version, "version", value_or_struct)
                 (&Self::opt_desc ,"opt_desc")
                 [attributes(default_{"default value"})]

                 (&Self::desc ,"desc")
                 .no_remain())
    std::string version;
    std::string desc;
    std::optional<std::string> opt_desc;
};

class test {
public:
    DERIVE_SERDE(test,
                 [attributes( default_{"hello"})]
                 (&Self::str, "str")
                 (&Self::i,   "i")
                 (&Self::vec, "vec")
                 [attributes(default_{tenum::OUTPUT}, to_lower, under_to_dash)]
                 (&Self::io,  "io")
                 [attributes(make_optional)]
                 (&Self::in,  "in")
                 [attributes(to_upper, under_to_dash)]
                 (&Self::pri, "pri")
                 (&Self::m ,  "m")
                 (&Self::nm , "nm"))
    std::optional<std::string> str;
    int i;
    std::optional<std::vector<std::string>> vec;
    tenum io;
    std::vector<nested> in;
    std::map<std::string, std::string> m;
    std::map<std::string, nested> nm;
    std::string pri;
};

int main()
{
    nlohmann::json v = R"({
    "i": 10,
    "vec": [ "one", "two", "three" ],
    "io" : "OUTPUT-2",
    "pri" : "PRi-FF",
    "in" : [{ "version" : "hello" }, "single"],
    "m" : { "a" : "1",
            "b" : "2",
            "c" : "3" },
    "nm" : { "a" : {"version" : "hello" },
            "b" : "hello2" }
    })"_json;

    // try {

    test t = serde::deserialize<test>(v);
    fmt::print("{}\n",serde::to_str(t.io));

    YAML::Node y = serde::serialize<YAML::Node>(10);
    std::cout << y << "\n";
    serde::deserialize<int>(y);


    auto v_to_json = serde::serialize<nlohmann::json>(t);
    auto v_to_toml = serde::serialize<serde::toml_v>(t);
    auto v_to_yaml = serde::serialize<serde::yaml>(t);
    auto v_to_rjson = serde::serialize<rapidjson::Document>(t);
    auto print = [](auto& doc) {
        using namespace rapidjson;
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);
        std::cout << "doc:" << buffer.GetString() << std::endl;
    };

    std::cout << "toml: " << v_to_toml << std::endl;
    fmt::print("json: {}\n", v_to_json.dump());
    std::cout << "yaml: " << v_to_yaml << std::endl;
    print(v_to_rjson);

    test t_from_toml = serde::deserialize<test>(v_to_toml);
    test t_from_yaml = serde::deserialize<test>(v_to_yaml);
    test t_from_rjson = serde::deserialize<test>(v_to_rjson);

    fmt::print("{}\n", t_from_toml);
    fmt::print("{}\n", t_from_yaml);
    fmt::print("{}\n", t_from_rjson);
    std::cout << t << '\n';


    return 0;
}

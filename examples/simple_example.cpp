#include <serdepp/serde.hpp>
#include <serdepp/adaptor/rapidjson.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>
#include <serdepp/adaptor/fmt.hpp>

using namespace serde::ostream;

std::string to_str(rapidjson::Document& doc) {
    using namespace rapidjson;
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
};

enum class tenum {INPUT , OUTPUT};

class test {
public:
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
        using namespace serde::attribute;
        using namespace std::literals;
        serde::serde_struct(context, value)
            (&test::str, "str")
            (&test::i,   "i")
            (&test::vec, "vec")
            [attributes(default_l({"a"s, "b"s}))]
            (&test::no_vec, "no_vec")
            (&test::io,  "io")
            [attributes(multi_key{"pri2"}, default_{"dd"})]
            (&test::pri, "pri")
            (&test::m ,  "m")
            ;
    }
    std::optional<std::string> str;
    int i;
    std::optional<std::vector<std::string>> vec;
    std::vector<std::string> no_vec;
    tenum io;
    std::map<std::string, std::string> m;
private:
    std::string pri;
};

int main()
{
    nlohmann::json v = R"({
    "i": 10,
    "vec": [ "one", "two", "three" ],
    "io": "INPUT",
    "pri2" : "pri",
    "m" : { "a" : "1",
            "b" : "2",
            "c" : "3" }
    })"_json;

    try {
        test t = serde::deserialize<test>(v);

        auto v_to_json  = serde::serialize<nlohmann::json>(t);
        auto v_to_rjson = serde::serialize<rapidjson::Document>(t);
        auto v_to_toml  = serde::serialize<toml::value>(t);
        auto v_to_yaml  = serde::serialize<YAML::Node>(t);

        std::cout << "toml: " << v_to_toml << std::endl;
        fmt::print("json: {}\n", v_to_json.dump());
        std::cout << "yaml: " << v_to_yaml << std::endl;
        fmt::print("rjson: {}\n", to_str(v_to_rjson));

        test t_from_toml = serde::deserialize<test>(v_to_toml);
        test t_from_yaml = serde::deserialize<test>(v_to_yaml);
        test t_from_rjson = serde::deserialize<test>(v_to_rjson);

        fmt::print("-----------\n");
        fmt::print("{}\n", t_from_rjson);
        fmt::print("{}\n", t_from_toml);
        fmt::print("{}\n", t_from_yaml);
        std::cout << t << '\n';

    }catch(std::exception& e) {
        fmt::print(stderr,"{}\n",e.what());
    }

    return 0;
}

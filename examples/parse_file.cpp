#include "serdepp/serde.hpp"

#include "serdepp/adaptor/nlohmann_json.hpp"
#include "serdepp/adaptor/rapidjson.hpp"
#include "serdepp/adaptor/toml11.hpp"
#include "serdepp/adaptor/yaml-cpp.hpp"
#include "serdepp/adaptor/fmt.hpp"

enum class tenum { INPUT, OUTPUT, INPUT_2 , OUTPUT_2 };

struct nested {
    DERIVE_SERDE(nested,
            (&Self::version, "version", value_or_struct)
            (&Self::opt_desc ,"opt_desc")
            (&Self::desc ,"desc", default_{"default value"})
            .no_remain())
    std::string version;
    std::string desc;
    std::optional<std::string> opt_desc;
};

class test {
public:
    DERIVE_SERDE(test,
            (&Self::str, "str", default_{"hello"})
            (&Self::i,   "i")
            (&Self::vec, "vec")
            (&Self::io,  "io", default_{tenum::OUTPUT}, to_lower, under_to_dash)
            (&Self::in,  "in", make_optional)
            (&Self::pri, "pri", to_upper, under_to_dash)
            (&Self::m ,  "m")
            (&Self::nm , "nm", make_optional))
    std::optional<std::string> str;
    int i;
    std::optional<std::vector<std::string>> vec;
    tenum io;
    std::vector<nested> in;
    std::map<std::string, std::string> m;
    std::map<std::string, nested> nm;
    std::string pri;
};

int main(int argc, char *argv[])
{
    auto nljson = serde::parse_file<nlohmann::json>("../../examples/test.json");
    auto rpjson = serde::parse_file<rapidjson::Document>("../../examples/test.json");
    auto toml11 = serde::parse_file<toml::value>("../../examples/test.toml");
    auto yamlpp = serde::parse_file<YAML::Node>("../../examples/test.yaml");

    auto from_nljson = serde::deserialize<test>(nljson);
    auto from_rpjson = serde::deserialize<test>(rpjson);
    auto from_toml11 = serde::deserialize<test>(toml11);
    auto from_yamlpp = serde::deserialize<test>(yamlpp);

    fmt::print("{}\n",from_nljson);
    fmt::print("{}\n",from_rpjson);
    fmt::print("{}\n",from_toml11);
    fmt::print("{}\n",from_yamlpp);
    return 0;
}

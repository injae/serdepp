#include "serdepp/adaptor/fmt.hpp"
#include "serdepp/utility.hpp"
#include "serdepp/adaptor/toml11.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"

struct author {
    derive_serde(author, ctx.tag(github,"github").tag(name, "name").tag(twitter, "twitter").no_remain();)
    std::string github;
    std::string name;
    std::string twitter;
};

struct convert {
    derive_serde(convert,
              ctx.tag(cpp, "cpp")
                 .tag(lib, "lib")
                 .tag(repo, "repo")
                 .tag(toml, "toml")
                 .tag(author, "author")
                 .no_remain();)

    std::vector<int> cpp;
    std::string lib;
    std::string repo;
    std::vector<std::string> toml;
    author author;
};

int main(int argc, char* argv[]) {
    std::string data = R"(
cpp = [ 17, 20, 22 ]
lib = 'toml++'
repo = 'https://github.com/marzer/tomlplusplus/'
toml = [ '1.0.0-rc.3', 'and beyond' ]

[author]
github = 'https://github.com/marzer'
name = 'Mark Gillard'
twitter = 'https://twitter.com/marzer8789'
)";
    std::istringstream stream(data);
    auto t = toml::parse(stream);

    auto from_toml = serde::serialize<convert>(t);
    fmt::print("{}\n",from_toml);

    auto to_json_with_serde = serde::deserialize_with_name<nlohmann::json>(from_toml, "convert");
    fmt::print("{}\n",to_json_with_serde.dump(4));



    return 0;
}


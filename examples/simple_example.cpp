#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/ostream.hpp>

#include <serdepp/attributes.hpp>

using namespace serde::ostream;

enum class tenum {
    INPUT ,
    OUTPUT,
};

class test {
public:
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
        using serde::attribute::set_default;
        serde::serde_struct(context, value)
            .field(&test::str, "str")
            .field(&test::i,   "i")
            .field(&test::vec, "vec")
            .field(&test::io,  "io")
            .field(&test::pri, "pri")
            .field(&test::m ,  "m")
            ;
    }
    std::optional<std::string> str;
    int i;
    std::optional<std::list<std::string>> vec;
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
"pri" : "pri",
"m" : { "a" : "1",
        "b" : "2",
        "c" : "3" }
})"_json;

  try {
    test t = serde::serialize<test>(v);

    auto v_to_json = serde::deserialize<nlohmann::json>(t);
    auto v_to_toml = serde::deserialize<serde::toml_v>(t);

    std::cout << "toml: " << v_to_toml << std::endl;
    fmt::print("json: {}\n", v_to_json.dump());

    test t_from_toml = serde::serialize<test>(v_to_toml);


    fmt::print("{}\n", t);
    std::cout << t << '\n';

  }catch(std::exception& e) {
      fmt::print(stderr,"{}\n",e.what());
  }

  return 0;
}

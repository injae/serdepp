#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/ostream.hpp>

#include <serdepp/attributes.hpp>

using namespace serde::ostream;

enum class tenum {
    INPUT  = 1,
    OUTPUT = 2,
};

struct nested {
    template<class Context>
    constexpr static auto serde(Context& context, nested& value) {
        using namespace serde::attribute;
        serde::serde_struct(context, value)
            .field(&nested::version, "version", value_or_struct{})
            .field(&nested::opt_desc ,"opt_desc")
            .field(&nested::desc ,"desc", set_default("default value"))
            .no_remain();
    }
    std::string version;
    std::string desc;
    std::optional<std::string> opt_desc;
};

class test {
public:
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
        using serde::attribute::set_default;
        serde::serde_struct(context, value)
            .field(&test::str, "str", set_default("hello"))
            .field(&test::i,   "i")
            .field(&test::vec, "vec")
            .field(&test::io,  "io")
            .field(&test::in,  "in")
            .field(&test::pri, "pri")
            .field(&test::m ,  "m")
            .field(&test::nm , "nm")
            ;
    }
    std::optional<std::string> str;
    int i;
    std::optional<std::vector<std::string>> vec;
    tenum io;
    std::vector<nested> in;
    std::map<std::string, std::string> m;
    std::map<std::string, nested> nm;
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
"in" : [{ "version" : "hello" }, "single"],
"m" : { "a" : "1",
        "b" : "2",
        "c" : "3" },
"nm" : { "a" : {"version" : "hello" },
         "b" : "hello2" }
})"_json;

  try {
  test t = serde::serialize<test>(v);
  t.str.value();

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

#include <benchmark/benchmark.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <functional>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <variant>
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/fmt.hpp>

enum class tenum {
    INPUT = 1,
    OUTPUT = 2,
};

struct test {

    template<class Context>
    static constexpr auto serde(Context& context, test& value) {
      return serde::serde_struct<Context, test, 3>(context, value)
          .field(&test::str, "str")
          .field(&test::i, "i")
          .field(&test::vec, "vec")
          .field(&test::io, "io");
    }
    std::optional<std::string> str;
    int i;
    std::optional<std::vector<std::string>> vec;
    tenum io;
};

int main()
{
    nlohmann::json v = R"({"str":"hello", "i": 10, "vec": [ "one", "two", "three" ], "io": "INPUT", "re" : "re"})"_json;
    test t = serde::serialize<test>(v);
    fmt::print("test:{{ str:{}, i:{}, vec:{} }}\n", t.str.value(), t.i, t.vec.value());

    auto v_to_json= serde::deserialize<nlohmann::json>(t);
    auto v_to_toml= serde::deserialize<serde::toml_v>(t);

    std::cout << "toml: " << v_to_toml << std::endl;
    fmt::print("json: {}\n", v_to_json.dump());

    test t_from_toml = serde::serialize<test>(v_to_toml);

    fmt::print("{}",t);


    return 0;
}

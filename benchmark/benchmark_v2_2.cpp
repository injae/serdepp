#include <benchmark/benchmark.h>
#include <fmt/format.h>
#include <any>
#include <functional>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <variant>
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>

struct test {
    template<class Context>
    static constexpr auto serde(Context& context, test& value) {
      return serde::serde_struct<Context, test, 3>(context, value)
          .field(&test::str, "str")
          .field(&test::i, "i")
          .field(&test::vec, "vec");
      //.no_remain();
    }
    std::string str;
    int i;
    std::vector<std::string> vec;
};

namespace ns {
    struct nl_test {
        std::string str;
        int i;
        std::vector<std::string> vec;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(nl_test, str, i, vec)
    };
}


static void nljson_set_se_bench(benchmark::State& state) {
    nlohmann::json v = R"({"str":"hello", "i": 10, "vec": [ "one", "two" ]})"_json;
    for(auto _ : state) {
        serde::serialize<test>(v);
    }
}

static void nljson_set_nl_bench(benchmark::State& state) {
    nlohmann::json v = R"({"str":"hello", "i": 10, "vec": [ "one", "two" ]})"_json;
    for(auto _ : state) {
        v.get<ns::nl_test>();
    }
}

static void nljson_get_se_bench(benchmark::State& state) {
    test t;
    t.i = 10;
    t.str = "hello";
    t.vec = {"one", "two"};
    for(auto _ : state) {
        serde::deserialize<nlohmann::json>(t);
    }
}

static void nljson_get_nl_bench(benchmark::State& state) {
    ns::nl_test t;
    t.i = 10;
    t.str = "hello";
    t.vec = {"one", "two"};
    for(auto _ : state) {
         nlohmann::json{t};
    }
}
using namespace toml::literals::toml_literals;
namespace ext
{
struct test
{
    std::string str;
    int i;
    std::vector<std::string> vec;
    template<typename C, template<typename ...> class M, template<typename ...> class A>
    void from_toml(const toml::basic_value<C, M, A>& v)
    {
        this->str = toml::find<std::string>(v, "str");
        this->i = toml::find<int>(v, "i");
        this->vec = toml::find<std::vector<std::string>>(v, "vec");
        return;
    }

    toml::value into_toml() const // you need to mark it const.
    {
        return toml::value{{"str", this->str}, {"i", this->i}, {"vec", this->vec}};
    }
};
} // ext

static void toml11_set_se_bench(benchmark::State& state) {
    toml::value v = R"(
        vec = ["one", "two"]
        i = 10
        str = "hello")"_toml;
    for(auto _ : state) {
        serde::serialize<test>(v);
    }
}

static void toml11_set_tl_bench(benchmark::State& state) {
    toml::value v = R"(
        vec = ["one", "two"]
        i = 10
        str = "hello"
    )"_toml;
    for(auto _ : state) {
        toml::get<ext::test>(v);
    }
}

static void toml11_get_se_bench(benchmark::State& state) {
    test t;
    t.i = 10;
    t.str = "hello";
    t.vec = {"one", "two"};
    for(auto _ : state) {
        serde::deserialize<serde::toml_v>(t);
    }
}

static void toml11_get_tl_bench(benchmark::State& state) {
    ext::test t;
    t.i = 10;
    t.str = "hello";
    t.vec = {"one", "two"};
    for(auto _ : state) {
        toml::value v(t);
    }
}

BENCHMARK(nljson_set_se_bench);
BENCHMARK(nljson_set_nl_bench);
BENCHMARK(nljson_get_se_bench);
BENCHMARK(nljson_get_nl_bench);
BENCHMARK(toml11_set_se_bench);
BENCHMARK(toml11_set_tl_bench);
BENCHMARK(toml11_get_se_bench);
BENCHMARK(toml11_get_tl_bench);
BENCHMARK_MAIN();

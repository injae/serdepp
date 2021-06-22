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

struct test {
    template<class Context>
    static constexpr auto serde(Context& context, test& value) {
      return serde::serde_struct<Context, test, 3>(context, value)
          .field(&test::str, "str")
          .field(&test::i, "i")
          .field(&test::vec, "vec");
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

BENCHMARK(nljson_set_se_bench);
BENCHMARK(nljson_set_nl_bench);
BENCHMARK(nljson_get_se_bench);
BENCHMARK(nljson_get_nl_bench);
BENCHMARK_MAIN();

#include <benchmark/benchmark.h>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/utility.hpp>
#include <nlohmann/json.hpp>
#include <string>

struct TestStruct {
    derive_serde(TestStruct, ctx.TAG(str).TAG(i).TAG(vec);)
    std::optional<std::string> str;
    int i;
    std::vector<std::string> vec;
};

namespace ns {
    struct TestStruct2 {
        std::string str;
        int i;
        std::vector<std::string> vec;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(TestStruct2, str, i, vec)
    };
}

static void serialize_struct_serde_nlohmann_json(benchmark::State& state) {
    using namespace serde;
    nlohmann::json test_json = R"({"str":"hello", "i": 10, "vec": [ "one", "two" ]})"_json;
    for(auto _ : state) {
        auto _v = serde::serialize<TestStruct>(test_json);
    }
}

static void serialize_struct_nlohmann_json(benchmark::State& state) {
    nlohmann::json test_json = R"({"str":"hello", "i": 10, "vec": [ "one", "two" ]})"_json;
    for(auto _ : state) {
       auto _v = test_json.get<ns::TestStruct2>();
    }
}

static void deserialize_serde_nlohmann_json(benchmark::State& state) {
    nlohmann::json test_json = R"({"str":"hello", "i": 10, "vec": [ "one", "two" ]})"_json;
    auto value = serde::serialize<TestStruct>(test_json);
    for(auto _ : state) {
        auto _v = serde::deserialize<nlohmann::json>(value);
    }
}

static void deserialize_nlohmann_json(benchmark::State& state) {
    nlohmann::json test_json = R"({"str":"hello", "i": 10, "vec": [ "one", "two" ]})"_json;
    auto value = test_json.get<ns::TestStruct2>();
    for(auto _ : state) {
        auto _v = nlohmann::json{value};
    }
}


static void serialize_int_serde_nlohmann_json(benchmark::State& state) {
    using namespace serde;
    nlohmann::json test_type_json = 10;
    for(auto _ : state) {
        auto _v = serde::serialize<int>(test_type_json);
    }
}

static void serialize_int_nlohmann_json(benchmark::State& state) {
    nlohmann::json test_type_json = 10;
    for(auto _ : state) {
       auto _v = test_type_json.get<int>();
    }
}

static void deserialize_int_serde_nlohmann_json(benchmark::State& state) {
    using namespace serde;
    int value = 1000;
    for(auto _ : state) {
        auto _v = serde::deserialize<nlohmann::json>(value);
    }
}

static void deserialize_int_nlohmann_json(benchmark::State& state) {
    int value = 1000;
    for(auto _ : state) {
        auto _v = nlohmann::json{value};
    }
}

nlohmann::json test_vec_json = { 10, 11, 12, 13 };

static void serialize_vec_serde_nlohmann_json(benchmark::State& state) {
    using namespace serde;
    for(auto _ : state) {
        auto _v = serde::serialize<std::vector<int>>(test_vec_json);
    }
}

static void serialize_vec_nlohmann_json(benchmark::State& state) {
    for(auto _ : state) {
       auto _v = test_vec_json.get<std::vector<int>>();
    }
}

static void deserialize_vec_serde_nlohmann_json(benchmark::State& state) {
    using namespace serde;
    std::vector<int> value = { 1, 2, 3, 4, 5};
    for(auto _ : state) {
        auto _v = serde::deserialize<nlohmann::json>(value);
    }
}

static void deserialize_vec_nlohmann_json(benchmark::State& state) {
    std::vector<int> value = { 1, 2, 3, 4, 5};
    for(auto _ : state) {
        auto _v = nlohmann::json{value};
    }
}

BENCHMARK(serialize_struct_serde_nlohmann_json);
BENCHMARK(serialize_struct_nlohmann_json);
BENCHMARK(deserialize_serde_nlohmann_json);
BENCHMARK(deserialize_nlohmann_json);
BENCHMARK(serialize_int_serde_nlohmann_json);
BENCHMARK(serialize_int_nlohmann_json);
BENCHMARK(deserialize_int_serde_nlohmann_json);
BENCHMARK(deserialize_int_nlohmann_json);
BENCHMARK(serialize_vec_serde_nlohmann_json);
BENCHMARK(serialize_vec_nlohmann_json);
BENCHMARK(deserialize_vec_serde_nlohmann_json);
BENCHMARK(deserialize_vec_nlohmann_json);

BENCHMARK_MAIN();

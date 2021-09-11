#include <benchmark/benchmark.h>
#include <serdepp/serde.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
struct Object {
    DERIVE_SERDE(Object,
                 (&Self::radius, "radius")
                 (&Self::width, "width")
                 (&Self::height, "height"))
    std::optional<int> radius;
    std::optional<int> width;
    std::optional<int> height;
};
struct Test {
    DERIVE_SERDE(Test,
                 (&Self::type, "type")
                 (&Self::object, "object", flatten)
                 )
    std::string type;
    Object object;
};

struct Test_v2 {
    DERIVE_SERDE(Test_v2,
                 (&Self::type, "type")
                 [attributes(flatten)]
                 (&Self::object, "object")
                 )
    std::string type;
    Object object;
};

nlohmann::json jflat = R"([{"type": "circle", "radius": 5}, {"type": "rectangle", "width": 6, "height": 5}])"_json;

nlohmann::json j = R"([{"type": "circle", "object": {"radius" : 5}},
 {"type": "rectangle", "object": {"width": 6, "height": 5}}])"_json;

static void set_with_sugar_bench(benchmark::State& state) {
    auto test_data = jflat;
    for(auto _ : state) {
        serde::deserialize<std::vector<Test>>(test_data);
    }
}

static void set_with_default_bench(benchmark::State& state) {
    auto test_data = jflat;
    for(auto _ : state) {
        serde::deserialize<std::vector<Test_v2>>(test_data);
    }
}

BENCHMARK(set_with_default_bench);
BENCHMARK(set_with_sugar_bench);
BENCHMARK_MAIN();

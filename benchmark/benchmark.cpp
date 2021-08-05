#include <benchmark/benchmark.h>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <variant>
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>
#include <serdepp/adaptor/rapidjson.hpp>
#include <fmt/format.h>
#include <serdepp/utility.hpp>

struct test {
    DERIVE_SERDE(test,
          (&Self::str, "str")
          (&Self::i,   "i")
          (&Self::vec, "vec")
          (&Self::sm,  "sm"))
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::map<std::string, std::string> sm;
};

namespace ns {
    struct nl_test {
    DERIVE_SERDE(test,
          (&Self::str, "str")
          (&Self::i,   "i")
          (&Self::vec, "vec")
          (&Self::sm,  "sm"))
        std::string str;
        int i;
        std::vector<std::string> vec;
        std::map<std::string, std::string> sm;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(nl_test, str, i, vec, sm)
    };
}


using namespace toml::literals;

nlohmann::json json_v = R"({
"str" : "hello",
"i": 10,
"vec": [ "one", "two", "three"],
"sm": { "one" : "tone", "two" : "ttwo"}
})"_json;

test base_t = serde::deserialize<test>(json_v);

//toml::value toml_v = R"(
//vec = ["one", "two"]
//i = 10
//str = "hello"
//[sm]
//one = "tone"
//two = "ttwo"
//)"_toml;


static void nljson_set_se_bench(benchmark::State& state) {
    auto test_data = json_v;
    for(auto _ : state) {
        serde::deserialize<test>(test_data);
    }
}

static void nljson_set_nl_bench(benchmark::State& state) {
    auto test_data = json_v;
    for(auto _ : state) {
        auto v = test_data.get<ns::nl_test>();
    }
}

static void nljson_get_se_bench(benchmark::State& state) {
    auto test_data = base_t;
    for(auto _ : state) {
        serde::serialize<nlohmann::json>(test_data);
    }
}

static void nljson_get_nl_bench(benchmark::State& state) {
    ns::nl_test t = serde::deserialize<ns::nl_test>(json_v);
    for(auto _ : state) {
         nlohmann::json{t};
    }
}

namespace ext
{
struct test
{
    DERIVE_SERDE(test,
          (&Self::str, "str")
          (&Self::i,   "i")
          (&Self::vec, "vec")
          (&Self::sm,  "sm"))
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::map<std::string, std::string> sm;
    template<typename C, template<typename ...> class M, template<typename ...> class A>
    void from_toml(const toml::basic_value<C, M, A>& v)
    {
        this->str = toml::find<std::string>(v, "str");
        this->i   = toml::find<int>(v, "i");
        this->vec = toml::find<std::vector<std::string>>(v, "vec");
        this->sm  = toml::find<std::map<std::string, std::string>>(v, "sm");
        return;
    }

    toml::value into_toml() const // you need to mark it const.
    {
        return toml::value{{"str", this->str}, {"i", this->i}, {"vec", this->vec}, {"sm", this->sm}};
    }
};
} // ext

static void toml11_set_se_bench(benchmark::State& state) {
    toml::value toml_v = serde::serialize<toml::value>(base_t);
    for(auto _ : state) {
        serde::deserialize<test>(toml_v);
    }
}

static void toml11_set_tl_bench(benchmark::State& state) {
    toml::value toml_v = serde::serialize<toml::value>(base_t);
    for(auto _ : state) {
        toml::get<ext::test>(toml_v);
    }
}

static void toml11_get_se_bench(benchmark::State& state) {
    auto test_data = base_t;
    for(auto _ : state) {
        serde::serialize<serde::toml_v>(test_data);
    }
}

static void toml11_get_tl_bench(benchmark::State& state) {
    ext::test t = serde::deserialize<ext::test>(json_v);
    for(auto _ : state) {
        toml::value v(t);
    }
}

namespace YAML {
    template<> struct convert<test> {
        static Node encode(const test& v) {
            Node node;
            node["str"] = v.str;
            node["i"] = v.i;
            node["vec"] = v.vec;
            node["sm"] = v.sm;
            return node;
        }

        static bool decode(const Node& node, test& v) {
            v.str = node["str"].as<std::string>();
            v.i   = node["i"].as<int>();
            v.vec = node["vec"].as<std::vector<std::string>>();
            v.sm  = node["sm"].as<std::map<std::string, std::string>>();
            return true;
        }
    };
}

static void yaml_set_se_bench(benchmark::State& state) {
    YAML::Node yaml_v = serde::serialize<YAML::Node>(base_t);
    for(auto _ : state) {
        serde::deserialize<test>(yaml_v);
    }
}

static void yaml_set_tl_bench(benchmark::State& state) {
    YAML::Node yaml_v = serde::serialize<YAML::Node>(base_t);
    for(auto _ : state) {
        yaml_v.as<test>();
    }
}

static void yaml_get_se_bench(benchmark::State& state) {
    auto test_data = base_t;
    for(auto _ : state) {
        serde::serialize<serde::yaml>(test_data);
    }
}

static void yaml_get_tl_bench(benchmark::State& state) {
    auto test_data = base_t;
    for(auto _ : state) {
        YAML::Node v(test_data);
    }
}

static void rapid_json_set_se_bench(benchmark::State& state) {
    rapidjson::Document rapid_v = serde::serialize<rapidjson::Document>(base_t);
    for(auto _ : state) {
        serde::deserialize<test>(rapid_v);
    }
}

[[maybe_unused]]
static void rapid_json_set_tl_bench(benchmark::State& state) {
    rapidjson::Document rapid_v = serde::serialize<rapidjson::Document>(base_t);
    for(auto _ : state) {
        //rapid_v.as<test>();
    }
}

static void rapid_json_get_se_bench(benchmark::State& state) {
    auto test_data = base_t;
    for(auto _ : state) {
        serde::serialize<rapidjson::Document>(test_data);
    }
}

[[maybe_unused]]
static void rapid_json_get_tl_bench(benchmark::State& state) {
    auto test_data = base_t;
    for(auto _ : state) {
        //RAPID_JSON::Node v(base_t);
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
BENCHMARK(yaml_set_se_bench);
BENCHMARK(yaml_set_tl_bench);
BENCHMARK(yaml_get_se_bench);
BENCHMARK(yaml_get_tl_bench);
BENCHMARK(rapid_json_set_se_bench);
//BENCHMARK(rapid_json_set_tl_bench);
BENCHMARK(rapid_json_get_se_bench);
//BENCHMARK(rapid_json_get_tl_bench);
BENCHMARK_MAIN();

#include <benchmark/benchmark.h>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <variant>
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>
#include <fmt/format.h>
#include <serdepp/utility.hpp>

using namespace toml::literals;

int64_t integer64 = 1024;
auto json_int64_t = serde::serialize<nlohmann::json>(integer64); 
static void nljson_set_int64_t_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<int64_t>(json_int64_t); } 
} 
static void nljson_set_int64_t_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { json_int64_t.get<int64_t>(); } 
} 
static void nljson_get_int64_t_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<nlohmann::json>(integer64); } 
} 
static void nljson_get_int64_t_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { nlohmann::json{integer64}; } 
} 
BENCHMARK(nljson_set_int64_t_se_bench);    
BENCHMARK(nljson_set_int64_t_nl_bench); 
BENCHMARK(nljson_get_int64_t_se_bench); 
BENCHMARK(nljson_get_int64_t_nl_bench); 
auto toml11_int64_t = serde::serialize<toml::value>(integer64); 
static void toml11_set_int64_t_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<int64_t>(toml11_int64_t); } 
} 
static void toml11_set_int64_t_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::get<int64_t>(toml11_int64_t); } 
} 
static void toml11_get_int64_t_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<toml::value>(integer64); }  
} 
static void toml11_get_int64_t_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::value v(integer64); } 
} 
BENCHMARK(toml11_set_int64_t_se_bench); 
BENCHMARK(toml11_set_int64_t_tm_bench); 
BENCHMARK(toml11_get_int64_t_se_bench); 
BENCHMARK(toml11_get_int64_t_tm_bench);
auto yaml_cpp_int64_t = serde::serialize<YAML::Node>(integer64); 
static void yaml_cpp_set_int64_t_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<int64_t>(yaml_cpp_int64_t); } 
} 
static void yaml_cpp_set_int64_t_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { yaml_cpp_int64_t.as<int64_t>(); } 
} 
static void yaml_cpp_get_int64_t_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<YAML::Node>(integer64); } 
} 
static void yaml_cpp_get_int64_t_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { YAML::Node{integer64}; } 
}
BENCHMARK(yaml_cpp_set_int64_t_se_bench); 
BENCHMARK(yaml_cpp_set_int64_t_ym_bench); 
BENCHMARK(yaml_cpp_get_int64_t_se_bench); 
BENCHMARK(yaml_cpp_get_int64_t_ym_bench); 


float float32 = 1024.1;
auto json_float = serde::serialize<nlohmann::json>(float32); 
static void nljson_set_float_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<float>(json_float); } 
} 
static void nljson_set_float_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { json_float.get<float>(); } 
} 
static void nljson_get_float_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<nlohmann::json>(float32); } 
} 
static void nljson_get_float_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { nlohmann::json{float32}; } 
} 
BENCHMARK(nljson_set_float_se_bench);    
BENCHMARK(nljson_set_float_nl_bench); 
BENCHMARK(nljson_get_float_se_bench); 
BENCHMARK(nljson_get_float_nl_bench); 
auto toml11_float = serde::serialize<toml::value>(float32); 
static void toml11_set_float_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<float>(toml11_float); } 
} 
static void toml11_set_float_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::get<float>(toml11_float); } 
} 
static void toml11_get_float_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<toml::value>(float32); }  
} 
static void toml11_get_float_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::value v(float32); } 
} 
BENCHMARK(toml11_set_float_se_bench); 
BENCHMARK(toml11_set_float_tm_bench); 
BENCHMARK(toml11_get_float_se_bench); 
BENCHMARK(toml11_get_float_tm_bench);
auto yaml_cpp_float = serde::serialize<YAML::Node>(float32); 
static void yaml_cpp_set_float_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<float>(yaml_cpp_float); } 
} 
static void yaml_cpp_set_float_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { yaml_cpp_float.as<float>(); } 
} 
static void yaml_cpp_get_float_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<YAML::Node>(float32); } 
} 
static void yaml_cpp_get_float_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { YAML::Node{float32}; } 
}
BENCHMARK(yaml_cpp_set_float_se_bench); 
BENCHMARK(yaml_cpp_set_float_ym_bench); 
BENCHMARK(yaml_cpp_get_float_se_bench); 
BENCHMARK(yaml_cpp_get_float_ym_bench); 

std::string std_string = "hello";
auto json_string = serde::serialize<nlohmann::json>(std_string); 
static void nljson_set_string_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::string>(json_string); } 
} 
static void nljson_set_string_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { json_string.get<std::string>(); } 
} 
static void nljson_get_string_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<nlohmann::json>(std_string); } 
} 
static void nljson_get_string_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { nlohmann::json{std_string}; } 
} 
BENCHMARK(nljson_set_string_se_bench);    
BENCHMARK(nljson_set_string_nl_bench); 
BENCHMARK(nljson_get_string_se_bench); 
BENCHMARK(nljson_get_string_nl_bench); 
auto toml11_string = serde::serialize<toml::value>(std_string); 
static void toml11_set_string_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::string>(toml11_string); } 
} 
static void toml11_set_string_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::get<std::string>(toml11_string); } 
} 
static void toml11_get_string_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<toml::value>(std_string); }  
} 
static void toml11_get_string_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::value v(std_string); } 
} 
BENCHMARK(toml11_set_string_se_bench); 
BENCHMARK(toml11_set_string_tm_bench); 
BENCHMARK(toml11_get_string_se_bench); 
BENCHMARK(toml11_get_string_tm_bench);
auto yaml_cpp_string = serde::serialize<YAML::Node>(std_string); 
static void yaml_cpp_set_string_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::string>(yaml_cpp_string); } 
} 
static void yaml_cpp_set_string_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { yaml_cpp_string.as<std::string>(); } 
} 
static void yaml_cpp_get_string_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<YAML::Node>(std_string); } 
} 
static void yaml_cpp_get_string_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { YAML::Node{std_string}; } 
}
BENCHMARK(yaml_cpp_set_string_se_bench); 
BENCHMARK(yaml_cpp_set_string_ym_bench); 
BENCHMARK(yaml_cpp_get_string_se_bench); 
BENCHMARK(yaml_cpp_get_string_ym_bench); 

std::vector<int> std_vector{1,2,3,4,5,6,7,8,9,10};
auto json_vector = serde::serialize<nlohmann::json>(std_vector); 
static void nljson_set_vector_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::vector<int>>(json_vector); } 
} 
static void nljson_set_vector_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { json_vector.get<std::vector<int>>(); } 
} 
static void nljson_get_vector_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<nlohmann::json>(std_vector); } 
} 
static void nljson_get_vector_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { nlohmann::json{std_vector}; } 
} 
BENCHMARK(nljson_set_vector_se_bench);    
BENCHMARK(nljson_set_vector_nl_bench); 
BENCHMARK(nljson_get_vector_se_bench); 
BENCHMARK(nljson_get_vector_nl_bench); 
auto toml11_vector = serde::serialize<toml::value>(std_vector); 
static void toml11_set_vector_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::vector<int>>(toml11_vector); } 
} 
static void toml11_set_vector_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::get<std::vector<int>>(toml11_vector); } 
} 
static void toml11_get_vector_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<toml::value>(std_vector); }  
} 
static void toml11_get_vector_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::value v(std_vector); } 
} 
BENCHMARK(toml11_set_vector_se_bench); 
BENCHMARK(toml11_set_vector_tm_bench); 
BENCHMARK(toml11_get_vector_se_bench); 
BENCHMARK(toml11_get_vector_tm_bench);
auto yaml_cpp_vector = serde::serialize<YAML::Node>(std_vector); 
static void yaml_cpp_set_vector_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::vector<int>>(yaml_cpp_vector); } 
} 
static void yaml_cpp_set_vector_ym_bench(benchmark::State& state) { 
    for(auto _ : state) {
        std::vector<int> vec;
        for (auto i = 0; i < yaml_cpp_vector.size(); ++i) 
            vec.push_back(yaml_cpp_vector[i].as<int>());
    } 
} 
static void yaml_cpp_get_vector_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<YAML::Node>(std_vector); } 
} 
static void yaml_cpp_get_vector_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { YAML::Node{std_vector}; } 
}
BENCHMARK(yaml_cpp_set_vector_se_bench); 
BENCHMARK(yaml_cpp_set_vector_ym_bench); 
BENCHMARK(yaml_cpp_get_vector_se_bench); 
BENCHMARK(yaml_cpp_get_vector_ym_bench); 

std::map<std::string, std::string> std_map{{"hello", "h"}, {"hello2", "h2"}, {"hello3", "h3"}};
auto json_map = serde::serialize<nlohmann::json>(std_map); 
static void nljson_set_map_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::map<std::string, std::string>>(json_map); } 
} 
static void nljson_set_map_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { json_map.get<std::map<std::string, std::string>>(); } 
} 
static void nljson_get_map_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<nlohmann::json>(std_map); } 
} 
static void nljson_get_map_nl_bench(benchmark::State& state) { 
    for(auto _ : state) { nlohmann::json{std_map}; } 
} 
BENCHMARK(nljson_set_map_se_bench);    
BENCHMARK(nljson_set_map_nl_bench); 
BENCHMARK(nljson_get_map_se_bench); 
BENCHMARK(nljson_get_map_nl_bench); 
auto toml11_map = serde::serialize<toml::value>(std_map); 
static void toml11_set_map_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::map<std::string, std::string>>(toml11_map); } 
} 
static void toml11_set_map_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::get<std::map<std::string, std::string>>(toml11_map); } 
} 
static void toml11_get_map_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<toml::value>(std_map); }  
} 
static void toml11_get_map_tm_bench(benchmark::State& state) { 
    for(auto _ : state) { toml::value v(std_map); } 
} 
BENCHMARK(toml11_set_map_se_bench); 
BENCHMARK(toml11_set_map_tm_bench); 
BENCHMARK(toml11_get_map_se_bench); 
BENCHMARK(toml11_get_map_tm_bench);
auto yaml_cpp_map = serde::serialize<YAML::Node>(std_map); 
static void yaml_cpp_set_map_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::deserialize<std::map<std::string, std::string>>(yaml_cpp_map); } 
} 
static void yaml_cpp_set_map_ym_bench(benchmark::State& state) { 
    for(auto _ : state) {
        std::map<std::string, std::string> map;
        for(YAML::const_iterator it = yaml_cpp_map.begin(); it!=yaml_cpp_map.end(); ++it) {
            auto key_ = it->first, value_ = it->second;
            map[key_.as<std::string>()] = value_.as<std::string>();
        }
    } 
} 
static void yaml_cpp_get_map_se_bench(benchmark::State& state) { 
    for(auto _ : state) { serde::serialize<YAML::Node>(std_map); } 
} 
static void yaml_cpp_get_map_ym_bench(benchmark::State& state) { 
    for(auto _ : state) { YAML::Node{std_map}; } 
}
BENCHMARK(yaml_cpp_set_map_se_bench); 
BENCHMARK(yaml_cpp_set_map_ym_bench); 
BENCHMARK(yaml_cpp_get_map_se_bench); 
BENCHMARK(yaml_cpp_get_map_ym_bench); 


BENCHMARK_MAIN();

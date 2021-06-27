# serdepp (Beta)
> c++17 zero cost serialize deserialize adaptor library like rust serde   
> Multiple types of serializer deserializer(json, toml, fmt(deserialize only)) can be created in one serializer function.  

## Features
- [x] zero cost serializer, deserializer adaptor
- [x] json serielize, deserialize (with nlohmann_json)
- [x] toml serielize, deserialize (with toml11)
- [x] yaml serielize, deserialize (with yaml-cpp)
- [x] fmt::format support 
- [x] std::cout(ostream) support (Beta)
- [x] struct, class support
- [x] nested struct, class support
- [x] enum, enum_class support (with magic_enum)
- [x] optional support 
- [x] container support (sequence(like vector, list), map(map, unordered_map ...))
- [x] attributes and custom attribute support (value_or_struct, set_default, ...)


## Dependencies
- fmt         (Auto Install)
- nameof      (Auto Install)
- magic_enum  (Auto Install)
- nlohmann_json (optional) (Auto Install CMAKE FLAG: -DSERDEPP_USE_NLOHMANN_JSON=ON)
- toml11 (optional) (Auto Install CMAKE FLAG: -DSERDEPP_USE_TOML11=ON)
- yaml-cpp (optional) (Auto Install CMAKE FLAG: -DSERDEPP_USE_YAML_CPP=ON)

## Install
```console
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release .
cd build
cmake --build . --config Release --target install
``
`

## CMake 
```cmake
find_package(serdepp)
target_link_libraries({target name} PUBLIC serdepp::serdepp)
```

## Examples (examples/*.cpp)
### Simple Example
```cpp
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/ostream.hpp>

using namespace serde::ostream;

enum class tenum {
    INPUT ,
    OUTPUT,
};

class test {
public:
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
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
    std::optional<std::vector<std::string>> vec;
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

    test t = serde::serialize<test>(v);

    auto v_to_json = serde::deserialize<nlohmann::json>(t);
    auto v_to_toml = serde::deserialize<serde::toml_v>(t);
    auto v_to_toml = serde::deserialize<serde::yaml>(t);

    test t_from_toml = serde::serialize<test>(v_to_toml);
    test t_from_yaml = serde::serialize<test>(v_to_yaml);

    fmt::print("{}\n", t);
    std::cout << t << '\n';

  return 0;
}
```


### Full Example
```cpp
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/attributes.hpp>

/// optional beta feature (for std::cout)
#include <serdepp/ostream.hpp>
using namespace serde::ostream; 
///

enum class tenum {
    INPUT  = 1,
    OUTPUT = 2,
};

struct nested {
    tem
plate<class Context>
    constexpr static auto serde(Context& context, nested& value) {
        using namespace serde::attribute;
        serde::serde_struct(context, value)
            .field(&nested::version, "version", value_or_struct{}) // value_or_struct attribute
            .field(&nested::opt_desc ,"opt_desc")
            .field(&nested::desc ,"desc", set_default("default value")) // serialize step set default value
            .no_remain();
    }
    std::string version;
    std::string desc;
    std::optional<std::string> opt_desc = "set opt default";
};

class test {
public:
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
        serde::serde_struct(context, value)
            .field(&test::str, "str")
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
  try {
    nlohmann::json v = R"({
    "str":"hello",
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

    // nlohmann::json -> class(test)
    test t = serde::serialize<test>(v);

    // class(test) -> nlohmann::json 
    auto v_to_json = serde::deserialize<nlohmann::json>(t);

    // class(test) -> toml11 
    auto v_to_toml = serde::deserialize<serde::toml_v>(t);

    // class(test) -> yaml-cpp
    auto v_to_yaml = serde::deserialize<serde::yaml>(t);

    // nlohmann::json -> string
    fmt::print("json: {}\n", v_to_json.dump());

    // toml11 -> string
    std::cout << "toml: " << v_to_toml << std::endl;


    // yaml-cpp -> string
    std::cout << "yaml: " << v_to_yaml << std::endl;

    // toml11 -> class(test)
    test t_from_toml = serde::serialize<test>(v_to_toml);

    // yaml-cpp -> class(test)
    test t_from_yaml = serde::serialize<test>(v_to_yaml);

    // class(test) -> string
    fmt::print("{}\n", t);

    // beta feature
    // need: #include <erdepp/ostream.hpp>
    // need: using namespace serdepp::ostream;
    // class(test) -> string
    std:cout << t << '\n';
    //

    } catch(std::exception& e) {
        fmt::print(stderr,"{}\n",e.what());
    }

  return 0;
}
```
# Attributes
## value_or_struct
```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, nested& value) {
        using namespace serde::attribute;
        serde::serde_struct(context, value)
            .field(&nested::version, "version", value_or_struct{});
    }
    std::string version;
};

```

## set_default
### support tree type default value serialzier
1. Type with Attribute set_default
2. std::optional Type with default
3. std::optional Type with Atribute set_default

```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, nested& value) {
        using namespace serde::attribute;
        serde::serde_struct(context, value)
            .field(&nested::ver, "ver", set_default{"0.0.1"}) // 1.
            .field(&nested::ver_opt, "ver_opt")               // 2.
            .field(&nested::ver_opt_defualt, "ver_opt_defalut", set_default{"0.0.1"}); // 3.
    }
    std::string version;
    std::optional<std::string> ver_opt = "0.0.1";
    std::optional<std::string> ver_opt_att_default;
};
```
## Custom Attribute
```
namespace serde::attribute {
    struct value_or_struct {
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_struct(ctx.adaptor)) {
                next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
            } else {
                next_attr.template from<T, serde_ctx>(ctx, data, "", remains...);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };
}
```



## Benchmark
- serdepp's serializer, deserizlier  vs nlohmann_json's serializer, deserizlier  
- serdepp's serializer, deserializer vs toml11 serializer, deserializer  
### Benchmark code, More(benchmark/benchmark.cpp)
```cpp
class test {
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::map<std::string, std::string> sm;
:public
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
      serde::serde_struct(context, value)
          .field(&test::str, "str")
          .field(&test::i, "i")
          .field(&test::vec, "vec")
          .field(&test::sm, "sm");
    }
};
```
### Result
```console
Running ./benchmark
Run on (12 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)
Load Average: 2.97, 2.97, 2.76
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
nljson_set_se_bench        453 ns          452 ns      1568968  // serde<nlohmann> serialize
nljson_set_nl_bench        447 ns          446 ns      1542571  // nlohmann serialize
nljson_get_se_bench       2369 ns         2365 ns       301281  // serde<nlohmann> struct deserialize
nljson_get_nl_bench       2733 ns         2729 ns       259948  // nlohmann struct deserialize
toml11_set_se_bench        525 ns          517 ns      1000000  // serde<toml11> struct serialize
toml11_set_tl_bench        546 ns          540 ns      1228372  // toml11 strcut serialize
toml11_get_se_bench       3516 ns         3509 ns       198763  // serde<toml11> struct deserialize
toml11_get_tl_bench       4191 ns         4181 ns       170082  // toml11 struct deserialize
yaml_set_se_bench         2096 ns         2089 ns       339427  // serde<yaml-cpp> struct serialize
yaml_set_tl_bench         2132 ns         2128 ns       323027  // yaml-cpp strcut serialize
yaml_get_se_bench        20363 ns        20323 ns        31356  // serde<yaml-cpp> struct deserialize
yaml_get_tl_bench        28897 ns        28847 ns        24286  // yaml-cpp struct deserialize
```



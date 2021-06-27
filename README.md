# serdepp (Beta)
> c++17 zero cost serialize deserialize adaptor library  
> this library inspired Rust serde  
> Multiple types of serializer deserializer(like json, toml, fmt(deserialize only)) can be created in one serializer function.  
> Adapters currently available (nlohmann::json, toml11, fmt)  
> You can easily add various formats.  
> support vairalbe type ( enum, optional, class, sequence, map, standard type, value_or_struct, nested_class)
> attribute support (value_or_struct, set_default)

## Dependencies
- fmt
- nameof
- magic_enum
- nlohmann_json (optional) (CMAKE FLAG: -DSERDEPP_USE_NLOHMANN_JSON=ON)
- toml11 (optional) (CMAKE FLAG: -DSERDEPP_USE_TOML11=ON)
- yaml-cpp (optional) (CMAKE FLAG -DSERDEPP_USE_YAML_CPP=ON)

# Examples

## Simple Example
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
    template<class Context>
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

    // nlohmann::json -> string
    fmt::print("json: {}\n", v_to_json.dump());

    // toml11 -> string
    std::cout << "toml: " << v_to_toml << std::endl;

    // toml11 -> class(test)
    test t_from_toml = serde::serialize<test>(v_to_toml);

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
## Attributes
### value_or_struct
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

### set_default
#### tree type default value serialzier
1. Type with Attribute set_default
2. Optional<Type> with default
3. Optional<Type> with Atribute set_default

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

## Benchmark
serdepp's serializer, deserizlier  vs nlohmann_json's serializer, deserizlier  
serdepp's serializer, deserializer vs toml11 serializer, deserializer  
### Benchmark 
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
2021-06-27T15:44:48+09:00
Running ./benchmark2
Run on (12 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)
Load Average: 3.04, 2.40, 2.33
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
nljson_set_se_bench        469 ns          466 ns      1436051
nljson_set_nl_bench        462 ns          459 ns      1539869
nljson_get_se_bench       2454 ns         2437 ns       290291
nljson_get_nl_bench       2826 ns         2793 ns       255620
toml11_set_se_bench        508 ns          504 ns      1320755
toml11_set_tl_bench        564 ns          560 ns      1228652
toml11_get_se_bench       3628 ns         3602 ns       194309
toml11_get_tl_bench       4187 ns         4149 ns       167348
```



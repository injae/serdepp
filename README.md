# serdepp
c++17 zero cost serialize deserialize adaptor library like rust serde.rs  

- [Features](#Features)
- [Get Started](#Get-Started)
  - [Dependencies](#Dependencies)
  - [Install](#Install)
  - [CMake](#CMake)
- [Examples](#Examples)
  - [Basic Usage](#Basic-Usage)
  - [Macro Version](#Macro-Version)
  - [Nested Class](#Nested-Class-Example)
  - [Custom Serializer](#Custom-Serializer)
- [Attributes](#Attributes)
  - [Naming Rule](#Nameing-Rule)
  - [Value or Struct](#value_or_struct_se)
  - [Default Value](#default_se)
  - [Enum](#enum_toupper)
  - [Container](#make_optional)
  - [Custom Attribute](#Custom-Attribute)
- [Benchmark](#Benchmark)

## Features
- [x] zero cost serializer, deserializer adaptor
- [x] json serialize, deserialize (with [nlohmann_json](https://github.com/nlohmann/json))
- [x] toml serialize, deserialize (with [toml11](https://github.com/ToruNiina/toml11))
- [x] yaml serialize, deserialize (with [yaml-cpp](https://github.com/jbeder/yaml-cpp))
- [x] [fmt](https://github.com/fmtlib/fmt) support 
- [x] std::cout(ostream) support (Beta)
- [x] struct, class support
- [x] nested struct, class support
- [x] enum, enum_class support (with [magic_enum](https://github.com/Neargye/magic_enum))
- [x] optional support 
- [x] container support (sequence(like vector, list), map(map, unordered_map ...))
- [x] [attributes](#Attributes) and custom attribute support (se_value_or_struct, se_default, ...)

## Get Started
```cpp
#include <serdepp/serde.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

enum class t_enum { A, B };

struct example {
    DERIVE_SERDE(example, 
                 (&Self::number_,  "number", skip{}) // attribute skip
                 (&Self::vec_,     "vec") 
                 (&Self::opt_vec_, "opt_vec")
                 (&Self::tenum_,   "t_enum")
                 //.no_remain() optional: if have unregiested data -> Exception
                 )
    int number_;
    std::vector<std::string> vec_;
    std::optional<std::vector<std::string>> opt_vec_;
    t_enum tenum_;
};  

int main() {
    example ex;
    ex.number_ = 1024;
    ex.vec_ = {"a", "b", "c"};
    ex.tenum_ = t_enum::B;

    nlohmann::json json_from_ex = serde::deserialize<nlohmann::json>(ex);
    example ex_from_json = serde::serialize<example>(json_from_ex);

    fmt::print("json:{}\n",json_from_ex.dump(4));

    fmt::print("fmt:{}\n",ex_from_json);
}

/* Result
json:{
    "t_enum": "B",
    "vec": [
        "a",
        "b",
        "c"
    ]
}
fmt:{"vec: {"a", "b", "c"}", "t_enum: B"}
*/
```

## Dependencies
- [fmt](https://github.com/fmtlib/fmt) (Auto Install)
- [nameof](https://github.com/Neargye/nameof) (Auto Install)
- [magic_enum](https://github.com/Neargye/magic_enum)  (Auto Install)
- [nlohmann_json](https://github.com/nlohmann/json) (optional) (Auto Install CMAKE FLAG: -DSERDEPP_USE_NLOHMANN_JSON=ON)
- [toml11](https://github.com/ToruNiina/toml11) (optional) (Auto Install CMAKE FLAG: -DSERDEPP_USE_TOML11=ON)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) (optional) (Auto Install CMAKE FLAG: -DSERDEPP_USE_YAML_CPP=ON)


## Install
```console
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release .
cd build
cmake --build . --config Release --target install
```

## CMake 
```cmake
find_package(serdepp)
target_link_libraries({target name} PUBLIC serdepp::serdepp)
```


# [Examples](./examples/)
## Basic Usage
```cpp
#include <serdepp/serializer.hpp>
class test {
public:
    template<class Context>
    constexpr static void serde(Context& context, test& value) {
        using Self = test;
        serde::serde_struct(context, value)
            (&Self::str, "str")  // or .field(&Self::str, "str")
            (&Self::i,   "i")    // or .field(&Self::i , "i")
            (&Self::vec, "vec"); // or .field(&Self::vec, "vec")
    }
private:
    std::string str;
    int i;
    std::vector<std::string> vec;
};
```

## Macro Version
```cpp
#include <serdepp/serializer.hpp>
class test {
public:
    DERIVE_SERDE(test,(&Self::str, "str")(&Self::i, "i")(&Self::vec, "vec"))
private:
    std::string str;
    int i;
    std::vector<std::string> vec;
};
```

## Custom Serializer
```cpp
struct Test {
    int i = 0;
};

template<typename serde_ctx>
    struct serde_serializer<Test, serde_ctx  /*SFINE Support*/> {
    // serialize step
    constexpr inline static auto from(serde_ctx& ctx, Test& data, std::string_view key) {
        // serialize int -> Test
        serde_adaptor<typename serde_ctx::Adaptor, int>::from(ctx.adaptor, key, data.i);
    }

    // deserialize step
    constexpr inline static auto into(serde_ctx& ctx, const Test& data, std::string_view key) {
        // deserialize  Test -> int
        serde_adaptor<typename serde_ctx::Adaptor, int>::into(ctx.adaptor, key, data.i);
    }
};
```

## [Simple Example](./examples/simple_example.cpp)
```cpp
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/ostream.hpp>

using namespace serde::ostream;

enum class tenum {
    INPUT,
    OUTPUT,
};

class test {
public:
    template<class Context>
    constexpr static auto serde(Context& context, test& value) {
        using Self = test;
        serde::serde_struct(context, value)
            .field(&Self::str, "str") // or (&test::str, "str")
            .field(&Self::i,   "i")
            .field(&Self::vec, "vec")
            .field(&Self::io,  "io")
            .field(&Self::pri, "pri")
            .field(&Self::m ,  "m")
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
    auto v_to_yaml = serde::deserialize<serde::yaml>(t);

    test t_from_toml = serde::serialize<test>(v_to_toml);
    test t_from_yaml = serde::serialize<test>(v_to_yaml);

    fmt::print("{}\n", t);
    std::cout << t << '\n';

  return 0;
}
```


### [Nested Class Example](./examples/example.cpp)
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
            .field(&nested::version, "version", value_or_struct_se{}) // value_or_struct attribute
            .field(&nested::opt_desc ,"opt_desc")
            .field(&nested::desc ,"desc", default_se("default value")) // serialize step set default value
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

## 3 Way make optional container field
1. with se_default 
   - if empty in serialize step -> set `std::vector<std::string>{}`
   - if empty in deserialize step -> set null, ex json: "vec" : null
2. with optional
   - if empty in serialize step -> set `std::nullopt`
   - if empty in deserialize step -> skip
3. with make_optional
   - if empty in serialize step -> set `std::vector<std::string>{}`
   - if empty in deserialize step -> skip
```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, attribute_example& value) {
        using namespace serde::attribute;
        using Self = attribute_example;
        serde::serde_struct(context, value)
            .field(&Self::vec, "vec", default_se<std::vector<std::string>>{{}}) // 1.
            .field(&Self::vec_opt, "vec_opt")       // 2.
            .field(&Self::vec_attr_opt, "vec_attr_opt", make_optional{});  // 3.
    }
    std::vector<std::string> ver;
    std::optional<std::vector<std::string>> vec_opt;
    std::vector<std::string> ver_att_opt;
};
```

# Attributes
## Naming Rule
- `*_se` serialize only     ex: `default_se` `value_or_struct_se`
- `*_de` deserialize only   ex: not yet
- `*` serialize deserialize ex: `enum_toupper`, `enum_tolower`

## `value_or_struct_se`
```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, nested& value) {
        using namespace serde::attribute;
        serde::serde_struct(context, value)
            .field(&nested::version, "version", value_or_struct_se{});
    }
    std::string version;
};
```

## `default_se`
### support tree type default value serializer
1. Type with Attribute default_se
2. std::optional Type with default
3. std::optional Type with Attribute default_se

```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, attribute_example& value) {
        using namespace serde::attribute;
        using Self = attribute_example;
        serde::serde_struct(context, value)
            .field(&Self::ver, "ver", default_se{"0.0.1"}) // 1.
            .field(&Self::ver_opt, "ver_opt")               // 2.
            .field(&Self::ver_opt_default, "ver_opt_default", default{"0.0.1"}_se); // 3.
    }
    std::string version;
    std::optional<std::string> ver_opt = "-1.0.1";
    std::optional<std::string> ver_opt_att_default;
};
```

## `enum_toupper` or `enum_tolower`
```cpp
enum class u_enum {
    INPUT ,
    OUTPUT,
}

enum class l_enum {
    input ,
    output,
}

struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, attribute_example& value) {
        using namespace serde::attribute;
        using Self = attribute_example;
        serde::serde_struct(context, value)
        // serialize:   input        -> INPUT -> uenum::INPUT
        // deserialize: uenum::INPUT -> INPUT -> input
            .field(&Self::test_uenum, "uenum", enum_toupper) 
        // serialize:   INPUT        -> input -> uenum::input
        // deserialize: uenum::input -> input -> INPUT
            .field(&Self::test_lenum, "lenum", enum_tolower);
    }
    u_enum test_uenum;
    l_enum test_lenum;
};
```

## `make_optional`
- c++ container make like optional type
- if empty in serialize step -> set `std::vector<std::string>{}`
- if empty in deserialize step -> not set
```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, attribute_example& value) {
        using namespace serde::attribute;
        using Self = attribute_example;
        serde::serde_struct(context, value)
            .field(&Self::vec, "vec", make_optional{});  // 3.
    }
    std::vector<std::string> ver;
};
```

## Custom Attribute
```cpp
// se_value_or_struct code in serde/attribute.hpp
namespace serde::attribute {
    struct value_or_struct_se {
        //serialize step
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

        //deserialize step
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes> 
        constexpr inline void into(serde_ctx& ctx, const T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };
}
```


## Benchmark
- serdepp serializer, deserializer  vs nlohmann_json's serializer, deserializer  
- serdepp serializer, deserializer vs toml11 serializer, deserializer  
### [Benchmark code](benchmark/benchmark.cpp)
```cpp
class test {
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::map<std::string, std::string> sm;
public:
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
2021-06-27T23:58:10+09:00
Running ./benchmark
Run on (12 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)
Load Average: 2.47, 2.43, 2.51
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
nljson_set_se_bench        425 ns          424 ns      1575076  // serde<nlohmann> serialize
nljson_set_nl_bench        442 ns          442 ns      1617058  // nlohmann serialize
nljson_get_se_bench       2277 ns         2274 ns       309051  // serde<nlohmann> struct deserialize
nljson_get_nl_bench       2610 ns         2606 ns       264174  // nlohmann struct deserialize
toml11_set_se_bench        466 ns          465 ns      1462581  // serde<toml11> struct serialize
toml11_set_tl_bench        499 ns          498 ns      1425691  // toml11 struct serialize
toml11_get_se_bench       3327 ns         3322 ns       208100  // serde<toml11> struct deserialize
toml11_get_tl_bench       4048 ns         4043 ns       175482  // toml11 struct deserialize
yaml_set_se_bench         1955 ns         1951 ns       355640  // serde<yaml-cpp> struct serialize
yaml_set_tl_bench         2044 ns         2042 ns       341966  // yaml-cpp struct serialize
yaml_get_se_bench        19619 ns        19592 ns        35286  // serde<yaml-cpp> struct deserialize
yaml_get_tl_bench        28810 ns        28754 ns        25024  // yaml-cpp struct deserialize
```



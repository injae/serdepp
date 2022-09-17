# Serdepp  
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/serdepp)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/serdepp?os&tab=overview)
[![Linux](https://github.com/injae/serdepp/actions/workflows/linux.yml/badge.svg)](https://github.com/injae/serdepp/actions/workflows/linux.yml) 
[![Windows](https://github.com/injae/serdepp/actions/workflows/window.yml/badge.svg)](https://github.com/injae/serdepp/actions/workflows/window.yml) 
[![MacOS](https://github.com/injae/serdepp/actions/workflows/macos.yml/badge.svg)](https://github.com/injae/serdepp/actions/workflows/macos.yml)
[![codecov](https://codecov.io/gh/injae/serdepp/branch/main/graph/badge.svg?token=2L217GF77B)](https://codecov.io/gh/injae/serdepp)  
c++17 low cost serialize deserialize adaptor library like rust serde.rs  
- [Features](#Features)
- [Get Started](#Get-Started)
  - [Dependencies](#Dependencies)
  - [Install With Vcpkg](#Install-With-Vcpkg)
  - [Install](#Install)
  - [CMake](#CMake)
- [Examples](#Examples)
  - [Basic Usage](#Basic-Usage)
  - [Macro Version](#Macro-Version)
  - [Nested Class](#Nested-Class-Example)
  - [Custom Serializer](#Custom-Serializer)
  - [Reflection](#Reflection)
- [Attributes](#Attributes)
  - [Value or Struct](#value_or_struct_se)
  - [Default Value](#default_se)
  - [Enum](#enum_toupper)
  - [Container](#make_optional)
  - [Custom Attribute](#Custom-Attribute)
- [Benchmark](#Benchmark)

## Features
- [x] low cost serializer, deserializer adaptor
- [x] json serialize, deserialize (with [rapidjson](https://github.com/Tencent/rapidjson), with [nlohmann_json](https://github.com/nlohmann/json))
- [x] toml serialize, deserialize (with [toml11](https://github.com/ToruNiina/toml11))
- [x] yaml serialize, deserialize (with [yaml-cpp](https://github.com/jbeder/yaml-cpp))
- [x] cli parser serialize        (with [CLI11](https://github.com/CLIUtils/CLI11))
- [x] string formatter [fmt](https://github.com/fmtlib/fmt) support 
- [x] std::cout(ostream) support
- [x] struct, class support
- [x] nested struct, class support
- [x] enum, enum_class support (with [magic_enum](https://github.com/Neargye/magic_enum))
- [x] optional support 
- [x] container support (sequence(like vector, list), map(map, unordered_map ...))
- [x] [attributes](#Attributes) and custom attribute support (value_or_struct, default, multi_key ...)
- [x] variant support (std::variant<int, std::vector<...>, UserType, EnumType...>) [example](examples/variant.cpp)
- [x] pointer support (T*, std::shared_ptr<T>, std::unique_ptr<T>)
- [x] [reflection](#Reflection) support 

## Serdepp Strcuture
![Serdepp structure](Serdepp_Structure.png)

## Get Started
```cpp
#include <serdepp/serde.hpp>

#include <serdepp/adaptor/nlohmann_json.hpp>

enum class t_enum { A, B };

struct example {
    DERIVE_SERDE(example, 
                 [attributes(skip)]
                 (&Self::number_,  "number") // attribute skip
                 (&Self::vec_,     "vec") 
                 (&Self::opt_vec_, "opt_vec")
                 (&Self::tenum_,   "t_enum")
                 //.no_remain() optional: if have unregisted data -> Exception
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
    //std::cout << ex << "\n";

    nlohmann::json json_from_ex = serde::serialize<nlohmann::json>(ex);
    example ex_from_json = serde::deserialize<example>(json_from_ex);

    std::cout << "json: " << json_from_ex.dump(4) << "\n";
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
- [nameof](https://github.com/Neargye/nameof) (Auto Install)
- [magic_enum](https://github.com/Neargye/magic_enum)  (Auto Install)


## Adaptor Libraries
- [fmt](https://github.com/fmtlib/fmt) with [fmt.hpp](./include/serdepp/adaptor/fmt.hpp)
- [nlohmann_json](https://github.com/nlohmann/json) with [nlohmann_json.hpp](./include/serdepp/adaptor/nlohmann_json.hpp)
- [rapidjson](https://github.com/Tencent/rapidjson) with [rapidjson.hpp](./include/serdepp/adaptor/rapidjson.hpp)
- [toml11](https://github.com/ToruNiina/toml11) with [toml11.hpp](./include/serdepp/adaptor/toml11.hpp)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) with [yaml-cpp.hpp](./include/serdepp/adaptor/yaml-cpp.hpp)
- [CLI11](https://github.com/CLIUtils/CLI11) with [cli11.hpp](./include/serdepp/adaptor/cli11.hpp), [attribute/cli11.hpp](./include/serdepp/attribute/cli11.hpp)

## Install With Vcpkg
```console
vcpkg install serdepp
# with other adaptors <nlohmann-json|toml11|yaml-cpp|fmt|rapidjson,cli11>
vcpkg install ${adaptor}

# with nlohmann-json adaptor
vcpkg install nlohmann-json

# with toml11 adaptor
vcpkg install toml11

# with yaml-cpp adaptor
vcpkg install yaml-cpp

# with fmt adaptor
vcpkg install fmt

# with rapidjson adaptor
vcpkg install rapidjson

# with cli11 adaptor
vcpkg install cli11
```
## Install
```console
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release .
cd build
cmake --build . --config Release --target install
```

## UnInstall
```console
cd build
cmake --build . --config Release --target uninstall
```

## CMake 
```cmake
find_package(serdepp)
target_link_libraries({target name} PUBLIC serdepp::serdepp)
```

## Compiler
- minimum compiler version clang-8, gcc-10

# [Examples](./examples/)
## Basic Usage
```cpp
#include "serdepp/serde.hpp"
#include "serdepp/adaptor/rapidjson.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"
#include "serdepp/adaptor/yaml-cpp.hpp"
#include "serdepp/adaptor/toml11.hpp"

int main(int argc, char *argv[])
{
    int num = 1; 

    auto rjson = serde::serialize<rapidjson::Document>(num);
    auto json = serde::serialize<nlohmann::json>(num);
    auto yaml = serde::serialize<YAML::Node>(num);
    auto toml = serde::serialize<toml::value>(num);

    int from_rjson = serde::deserialize<int>(rjson);
    int from_json  = serde::deserialize<int>(json);
    int from_toml  = serde::deserialize<int>(toml);
    int from_yaml  = serde::deserialize<int>(yaml);

    auto rjson_from_file = serde::parse_file<rapidjson::Document>("test.json");
    auto json_from_file  = serde::parse_file<nlohmann::json>("test.json");
    auto toml_from_file  = serde::parse_file<toml::value>("test.toml");
    auto yaml_from_file  = serde::parse_file<YAML::Node>("test.yaml");
    
    return 0;
}
```

## Define Struct Serializer
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
    DERIVE_SERDE(test, (&Self::str, "str")(&Self::i, "i")(&Self::vec, "vec"))
    // Macro Version
    // DERIVE_SERDE(test, _SF_(str)_SF_(i)_SF_(vec))
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
    struct serde_serializer<Test, serde_ctx  /*, SFINE*/> {
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

## [Reflection](./example/reflection.cpp)
```cpp

#include <serdepp/adaptor/reflection.hpp>
#include <serdepp/serde.hpp>
#include <serdepp/adaptor/reflection.hpp>

struct A {
    DERIVE_SERDE(A, (&Self::a, "a")
                    (&Self::b, "b")
                    (&Self::c, "c")
                    (&Self::d, "d")
                    (&Self::e, "e"))
    int a;
    std::string b;
    double c;
    std::vector<int> d;
    int e;
};

int main(int argc, char* argv[]) {
    constexpr auto info = serde::type_info<A>
;
    static_assert(serde::type_info<A>.size == 5);
    static_assert(serde::tuple_size_v<A> == 5);
    static_assert(std::is_same_v<serde::to_tuple_t<A>, std::tuple<int, std::string, double, std::vector<int>, int>>);
    static_assert(std::is_same_v<int, std::tuple_element_t<0, serde::to_tuple_t<A>>>);
    constexpr std::string_view a_name = info.name;

    auto a = A{1, "hello", 3.};

    auto to_tuple = serde::make_tuple(a);

    std::string& member_a = info.member<1>(a);
    member_a = "why";

    double& member_b_info = info.member<double>(a, "c");
    member_b_info = 3.14;

    auto member_d_info = info.member_info<3>(a);
    std::string_view member_d_name = member_d_info.name();
    std::vector<int>& member_d = member_d_info.value();

    auto names = info.member_names();
    for(auto& name : names.members()) {
        std::cout << name << "\n";
    }

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

enum class tenum {
    INPUT  = 1,
    OUTPUT = 2,
};

struct nested {
    DERIVE_SERDE(nested,
            [attributes(value_or_struct)]
            (&nested::version, "version") // value_or_struct attribute
            (&nested::opt_desc,"opt_desc")
            [attributes(default_{"default value"})]
            (&nested::desc ,"desc") // serialize step set default value
            .no_remain())
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
    test t = serde::deserialize<test>(v);

    // class(test) -> nlohmann::json 
    auto v_to_json = serde::serialize<nlohmann::json>(t);

    // class(test) -> toml11 
    auto v_to_toml = serde::serialize<serde::toml_v>(t);

    // class(test) -> yaml-cpp
    auto v_to_yaml = serde::serialize<serde::yaml>(t);

    // nlohmann::json -> string
    fmt::print("json: {}\n", v_to_json.dump());

    // toml11 -> string
    std::cout << "toml: " << v_to_toml << std::endl;


    // yaml-cpp -> string
    std::cout << "yaml: " << v_to_yaml << std::endl;

    // toml11 -> class(test)
    test t_from_toml = serde::deserialize<test>(v_to_toml);

    // yaml-cpp -> class(test)
    test t_from_yaml = serde::deserialize<test>(v_to_yaml);

    // class(test) -> string
    fmt::print("{}\n", t);

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
1. with default_
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
    DERIVE_SERDE(attribute_example,
                 [attributes(default_<std::vector<std::string>>{{}})] // 1
                 (&Self::vec, "vec") .
                 (&Self::vec_opt, "vec_opt")       // 2.
                 [attributes(make_optional)] // 3.
                 (&Self::vec_attr_opt, "vec_attr_opt")
                 )
    std::vector<std::string> ver;
    std::optional<std::vector<std::string>> vec_opt;
    std::vector<std::string> ver_att_opt;
};
```

# Attributes
## Two Way of Attributes add
## normal
```cpp
struct attribute_example {
    DERIVE_SERDE(attribute_example,
                 (&nested::version, "version", value_or_struct, default_("0.0.1"))) 

    std::string version;
};
```
## with syntax suger (Recommanded)
```cpp
struct attribute_example {
    DERIVE_SERDE(attribute_example,
                 [attributes(value_or_struct, default_("0.0.1"))]
                 (&nested::version, "version")) 

    std::string version;
};
```

## `value_or_struct`
```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, nested& value) {
        using namespace serde::attribute;
        serde::serde_struct(context, value)
            .field(&nested::version, "version", value_or_struct);
    }
    std::string version;
};
```

## `default_`
### support tree type default value serializer
1. Type with Attribute default_
2. std::optional Type with default
3. std::optional Type with Attribute default_

```cpp
struct attribute_example {
    template<class Context>
    constexpr static auto serde(Context& context, attribute_example& value) {
        using namespace serde::attribute;
        using Self = attribute_example;
        serde::serde_struct(context, value)
            .field(&Self::ver, "ver", default_{"0.0.1"}) // 1.
            .field(&Self::ver_opt, "ver_opt")               // 2.
            .field(&Self::ver_opt_default, "ver_opt_default", default_{"0.0.1"}); // 3.
    }
    std::string version;
    std::optional<std::string> ver_opt = "-1.0.1";
    std::optional<std::string> ver_opt_att_default;
};
```

## `toupper` or `tolower`
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
            .field(&Self::test_uenum, "uenum", to_upper) 
        // serialize:   INPUT        -> input -> uenum::input
        // deserialize: uenum::input -> input -> INPUT
            .field(&Self::test_lenum, "lenum", to_lower);
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
            .field(&Self::vec, "vec", make_optional);  // 3.
    }
    std::vector<std::string> ver;
};
```

## [more attribute](./include/serdepp/attribute/)
- `multi_key{...str}`
   - description: multiple key
   - args: initialize_list<std::string_view>
   - example: `(&Self::test, "key", mutli_key{"key2", "key3"})`
- `skip` 
   - description: skip serialize, deserialize step
   - example: `(&Self::test, "key", skip)`
- `skip_de`
   - description: skip deserialize step
   - example: `(&Self::test, "key", skip_de)`
- `skip_se`
   - description: skip serialize step
   - example: `(&Self::test, "key", skip_se)`
- `to_upper`
   - description: enum or string -> upper, upper string -> lower enum or string
   - example: `(&Self::test, "key", to_upper)` Enum::test -> TEST -> Enum::test
- `to_lower`
   - description: enum or string -> lower, lower string -> upper enum or string
   - example: `(&Self::test, "key", to_lower)` Enum::TEST -> test -> Enum::test
- `under_to_dash`
   - description: enum or string -> `_` -> `-` , `-` -> `_` enum or string
   - example: `(&Self::test, "key", under_to_dash)` Enum::TEST_TEST -> TEST-TEST -> Enum::TEST_TEST
- `defualt_`
   - description: parse like optional value
   - example: `(&Self::test, "key", default_{"default value"})` if null -> set default 
- `value_or_struct`
   - description: parse struct or single value, require other field default_ or optional
   - example: `(&Self::test, "key", value_or_struct)` "T": "value" or "T" : { "key" : "value" }
- `flatten`
   - description: parse struct flatten
   - example: `(&Self::test, "key", flatten)`  
   - { "obj" : {"key" : "value", "key2" : "value"} } == { "key" : "value", "key2" : "value" }
## Custom Attribute
### 1. Normal Attribute
```cpp
// value_or_struct code in serde/attribute/value_or_struct.hpp
namespace serde::attribute {
    namespace detail {
        struct value_or_struct {
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
    constexpr static auto value_or_struct = value_or_struct{};
}
```
### 2. Args Attribute
```cpp
// default_se code in serde/attribute/default.hpp
namespace serde::attribute {
    template<typename D>
    struct default_ {
        D&& default_value_;
        explicit default_(D&& default_value) noexcept : default_value_(std::move(default_value)) {}
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            using Helper = serde_adaptor_helper<typename serde_ctx::Adaptor>;
            if(Helper::is_null(ctx.adaptor, key)) {
                data = std::move(default_value_);
            } else {
                next_attr.template from<T, serde_ctx>(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                   Next&& next_attr, Attributes&&... remains) {
            next_attr.template into<T, serde_ctx>(ctx, data, key, remains...);
        }
    };
    // deduce guide
    template<typename D> default_(D&&) -> default_<D>;
}
```

## Serdepp Type Declare Rule

### Sequence Type<T>
- like vector , list, 
- require:
  - T.begin()
  - T.end()

### Map Type<T>
- like map, unordered_map
- require:
  - T::key_type
  - T::mapped_type
  - T.operator[](T::key_type&)
  
### Struct Type<T>
- require:
  - template<class Format> void serde(Format& formst, T& value);

 ## Benchmark
### Benchmark [Benchmark code](benchmark/benchmark.cpp)
```console
2021-08-05T21:32:23+09:00
Running ./benchmark
Run on (12 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)
Load Average: 2.52, 3.33, 3.15
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
nljson_set_se_bench            475 ns          474 ns      1306580
nljson_set_nl_bench            475 ns          472 ns      1550961
nljson_get_se_bench           2536 ns         2529 ns       275437
nljson_get_nl_bench           2768 ns         2764 ns       255292
toml11_set_se_bench            470 ns          469 ns      1496340
toml11_set_tl_bench            486 ns          485 ns      1418454
toml11_get_se_bench           3582 ns         3575 ns       195280
toml11_get_tl_bench           4194 ns         4189 ns       166580
yaml_set_se_bench             2091 ns         2088 ns       332965
yaml_set_tl_bench             2439 ns         2435 ns       285903
yaml_get_se_bench            25643 ns        25584 ns        26873
yaml_get_tl_bench            30182 ns        30155 ns        23070
rapid_json_set_se_bench        398 ns          397 ns      1743184
rapid_json_get_se_bench       2099 ns         2096 ns       331971
```

# serdepp  (Beta)
> c++17 serialize deserialize interface library  
> this library inspired Rust serde  
> Multiple types of serializer deserializer(like json, toml, fmt(deserialize only)) can be created in one function.  
> Adapters currently available (nlohmann::json, toml11,)  
> You can easily add various formats.  

## Dependencies
- fmt
- nlohmann_json (optional) (CMAKE FLAG: -DSERDEPP_USE_NLOHMANN_JSON=ON)
- toml11 (optional) (CMAKE FLAG: -DSERDEPP_USE_TOML11=ON)

# Examples

## Simple Example

```cpp
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/utility.hpp>
#include "serdepp/adaptor/nlohmann_json.hpp"
class bbb {
    derive_serde(bbb, ctx.tag(str, "str")
                         .tag(x, "x", 10) // with default value ( need  to std::optional type)
                         .tag(m, "m")     // if not this key panic
                         .tag(om, "om")
                         .tag(v, "v")
                         .no_remain(); // remain data check
                  )
public:
    std::string str; 
    std::optional<int> x ; //optional argument
    std::vector<std::string> v;
    std::unordered_map<std::string, std::string> m;
    std::optional<std::unordered_map<std::string, std::string>> om;
private:
    std::string z;
};

int main(int argc, char* argv[]) {
    nlohmann::json jctx = R"({"str":"ssss","v": ["h","e","l", "l", "o"],"m":{"a":"b","c":"d"}})"_json; 

    bbb json_to_bbb = serde::serialize<bbb>(jctx);
    fmt::print("{}\n",json_to_bbb); //with fmt

    nlohmann::json bbb_to_json = serde::deserialize<nlohmann::json>(json_to_bbb);
    fmt::print("{}\n",bbb_to_json.dump()); //with json
    return 0;
}

```

## Define serializer
```cpp
// define.h define struct and serialer, deserialer
#include <serdepp/serializer.hpp>


class in {
    derive_serde(in, ctx.tag(text, "text");)
    std::string text;
};

class bbb {
    derive_serde(bbb, ctx.tag(str, "str")
                         .tag(x, "x", 10) // with default value ( need  to std::optional type)
                         .tag(m, "m")     // if not this key panic
                         .tag(om, "om")
                         .tag(v, "v")
                         .no_remain(); // remain data check
                  )
public:
    std::string str; 
    std::optional<int> x ; //optional argument
    std::vector<std::string> v;
    std::unordered_map<std::string, std::string> m;
    std::optional<std::unordered_map<std::string, std::string>> om;
private:
    std::string z;
};

class ttt {
    // macro version <- need to #include<serdepp/utility.hpp>
    derive_serde(ttt, 
        ctx.tag(bbb, "bbb")
           .tag(inmap, "inmap")
           .tag(str, "str");
    )
    // no macro version
    //friend serde::serializer<ttt>; 
    //template<typename S> auto serde(S& ctx) {
    //    ctx.tag(bbb, "bbb"); // nested  struct
    //}
    
    // fmt support (need to #include<serdepp/adaptor/fmt.hpp>)
    //friend std::ostream& operator<<(std::ostream& os, ttt& type) { 
    //    return os << serde::deserialize<serde::string_convertor>(type, "ttt").to_string();
    //}
public:
    int x;
    std::optional<std::string> str;
    bbb bbb;
    std::vector<std::string> v;
    std::optional<std::unordered_map<std::string,in>> inmap;
private:
};
```

## Type Converter
```
// example class
class Test {
    void from_string(const std::string& str);
    void to_string(const std::string& str);
};

// serde serialize (std::string -> Test) deserialize (Test -> std::string)
template<> struct serializer<Test> : serializer_convertor<Test, std::string> { 
    using FROM = Test;
    using INTO = std::string;
    static void from(FROM& from_, INTO& into_) { from_.from_string(into_); }
    static void into(INTO& into_, FROM& from_) { into_ = from_.to_string(); }
}; 
 
```

## toml11 Example
```cpp
#include "define.h"
#include <serdepp/adaptor/toml11.hpp>

int main(int argc, char* argv[]) {
    std::string data = R"(
[ttt]
    [ttt.inmap]
        x.text="hello"
        y.text="world"
    [ttt.bbb]
        str = "ssss"
        v = [ "h", "e", "l", "l", "o" ]
        m = {a="b", c="d"}
)"; 
    std::istringstream stream(data);
    auto t = toml::parse(stream);
    fmt::print("{}\n",t);
    auto xx = serde::serialize_element<ttt>(t, "ttt");
auto yy = serde::deserialize_with_name<toml::value>(xx, "ttt");
    std::cout << yy << std::endl;

    return 0;
}
```


## Multiple format convert Example toml ->json , string
```cpp
#include <serdepp/serializer.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/utility.hpp>
#include <serdepp/adpator/toml11.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

struct author {
    derive_serde(author, ctx.tag(github,"github").tag(name, "name").tag(twitter, "twitter").no_remain();)
    std::string github;
    std::string name;
    std::string twitter;
};

struct convert {
    derive_serde(convert,
              ctx.tag(cpp, "cpp")
                 .tag(lib, "lib")
                 .tag(repo, "repo")
                 .tag(toml, "toml")
                 .tag(author, "author")
                 .no_remain();)

    std::vector<int> cpp;
    std::string lib;
    std::string repo;
    std::vector<std::string> toml;
    author author;
};

int main(int argc, char* argv[]) {
    std::string data = R"(
cpp = [ 17, 20, 22 ]
lib = 'toml++'
repo = 'https://github.com/marzer/tomlplusplus/'
toml = [ '1.0.0-rc.3', 'and beyond' ]

[author]
github = 'https://github.com/marzer'
name = 'Mark Gillard'
twitter = 'https://twitter.com/marzer8789'
)";
    std::istringstream stream(data);
    auto t = toml::parse(stream);

    auto from_toml = serde::serialize<convert>(t);
    fmt::print("{}", from_toml);

    auto to_json_with_serde = serde::deserialize_with_name<nlohmann::json>(from_toml, "convert");
    fmt::print("{}\n",to_json_with_serde.dump(4));


    return 0;
}

# result
/*
convert:{
    cpp:{17, 20, 22}
    lib:toml++
    repo:https://github.com/marzer/tomlplusplus/
    toml:{"1.0.0-rc.3", "and beyond"}
    author:{
        github:https://github.com/marzer
        name:Mark Gillard
        twitter:https://twitter.com/marzer8789
   }
}

{
    "convert": {
        "author": {
            "github": "https://github.com/marzer",
            "name": "Mark Gillard",
            "twitter": "https://twitter.com/marzer8789"
        },
        "cpp": [
            17,
            20,
            22
        ],
        "lib": "toml++",
        "repo": "https://github.com/marzer/tomlplusplus/",
        "toml": [
            "1.0.0-rc.3",
            "and beyond"
        ]
    }
}


*/

```

## FMT Example
```cpp
#include <serdepp/adaptor/fmt.hpp> // if you can use macro, must pre include before serdepp/utility.hpp
#include "define.h"
#include "serdepp/adaptor/toml11.hpp"

int main(int argc, char* argv[]) {
    std::string data = R"(
[ttt]
    [ttt.inmap]
        x.text="hello"
        y.text="world"
    [ttt.bbb]
        str = "ssss"
        v = [ "h", "e", "l", "l", "o" ]
        m = {a="b", c="d"}
)"; 
    std::istringstream stream(data);
    auto t = toml::parse(stream);
    fmt::print("{}\n",t);
    ttt xx = serde::serialize_element<ttt>(t, "ttt");
    fmt::print("{}\n",xx); // <---------------------- you can serialize class -> string

    toml::value yy = serde::deserialize_with_name<toml::value>(xx, "ttt");
    std::cout << yy << std::endl;


}
```
## Benchmark
serdepp's serializer vs nlohmann_json's serialer
```
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
```
```console
2021-02-06T04:16:41+09:00
Running ./benchmark
Run on (12 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)
Load Average: 2.20, 2.20, 2.23
-------------------------------------------------------------------------------
Benchmark                                     Time             CPU   Iterations
-------------------------------------------------------------------------------
serialize_struct_serde_nlohmann_json        255 ns          254 ns      2784906
serialize_struct_nlohmann_json              169 ns          168 ns      4616775
deserialize_serde_nlohmann_json            1848 ns         1842 ns       424721
deserialize_nlohmann_json                  1916 ns         1910 ns       407055
serialize_int_serde_nlohmann_json          3.40 ns         3.38 ns    251813054
serialize_int_nlohmann_json                3.05 ns         3.04 ns    252797931
deserialize_int_serde_nlohmann_json        7.41 ns         7.39 ns    105844107
deserialize_int_nlohmann_json               322 ns          321 ns      2406284
serialize_vec_serde_nlohmann_json           103 ns          102 ns      7604150
serialize_vec_nlohmann_json                 114 ns          113 ns      6926373
deserialize_vec_serde_nlohmann_json         997 ns          995 ns       797912
deserialize_vec_nlohmann_json              1122 ns         1119 ns       713405
```



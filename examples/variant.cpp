#include <serdepp/serde.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/toml11.hpp>
#include <serdepp/adaptor/yaml-cpp.hpp>

enum class T { A, B };

struct Rect {
    DERIVE_SERDE(Rect, (&Self::width, "width")(&Self::height, "height"))
    int width;
    int height;
};

struct Circle {
    DERIVE_SERDE(Circle, (&Self::radius, "radius"))
    int radius;
};
struct Test {
    DERIVE_SERDE(Test,
                 (&Self::type, "type")
                 (&Self::object, "object", flatten))
    std::string type;
    std::variant<Circle, Rect> object;
};

int main() {
    using namespace serde;

    nlohmann::json jflat = R"([
    {
        "type": "circle",
        "radius": 5
    },
    {
        "type": "rectangle",
        "width": 6,
        "height": 5
    }])"_json;

    nlohmann::json j = R"([
    {
       "type": "circle",
       "object": { 
          "radius" : 5
       }
    },
    {
       "type": "rectangle",
       "object": {
          "width": 6,
          "height": 5
        }
    }])"_json;

    auto j_flatten = deserialize<std::vector<Test>>(jflat);
    auto j_none = deserialize<std::vector<Test>>(j);

    fmt::print("{}\n",serialize<nlohmann::json>(j_flatten).dump(4));
    fmt::print("------\n");
    fmt::print("{}\n",serialize<nlohmann::json>(j_none).dump(4));
    fmt::print("------\n");
    auto tflat = serialize<toml::value>(j_flatten, "arr");
    auto t = serialize<toml::value>(j_none, "arr");
    std::cout << tflat << "\n";
    std::cout << t << "\n";
    fmt::print("------\n");
    auto yflat = serialize<YAML::Node>(j_flatten, "arr");
    auto y = serialize<YAML::Node>(j_none, "arr");
    std::cout << yflat << "\n";
    std::cout << y << "\n";
    fmt::print("------\n");
    fmt::print("{}\n", deserialize<std::vector<Test>>(tflat, "arr"));
    fmt::print("{}\n", deserialize<std::vector<Test>>(t, "arr"));
    fmt::print("------\n");
    fmt::print("{}\n", deserialize<std::vector<Test>>(yflat, "arr"));
    fmt::print("{}\n",deserialize<std::vector<Test>>(y, "arr"));

    std::variant<int, std::string, double, T,
                 std::vector<std::string>,
                 std::map<std::string, int>> var = T::A;
    auto v_y = serialize<YAML::Node>(var);
    auto v_t = serialize<toml::value>(var);
    auto v_j = serialize<nlohmann::json>(var);
    std::cout << v_y << "\n";
    std::cout << v_t << "\n";
    std::cout << v_j << "\n";
    fmt::print("{}\n", serde::to_string(deserialize<decltype(var)>(v_y)));
    fmt::print("{}\n", serde::to_string(deserialize<decltype(var)>(v_t)));
    fmt::print("{}\n", serde::to_string(deserialize<decltype(var)>(v_j)));
}

//OUTPUT
/*
[
    {
        "object": {
            "radius": 5
        },
        "type": "circle"
    },
    {
        "object": {
            "height": 5,
            "width": 6
        },
        "type": "rectangle"
    }
]
[
    {
        "object": {
            "radius": 5
        },
        "type": "circle"
    },
    {
        "object": {
            "height": 5,
            "width": 6
        },
        "type": "rectangle"
    }
]
*/


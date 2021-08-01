#include <serdepp/serde.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

enum class T { A, B};

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

    nlohmann::json jflat = R"([
    {
        "type": "circle",
        "radius": 5
    },
    {
        "type": "rectangle",
        "width": 6,
        "height": 5
    }
    ])"_json;

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

    auto j_flatten = serde::deserialize<std::vector<Test>>(jflat);
    auto j_none = serde::deserialize<std::vector<Test>>(j);

    fmt::print("{}\n",serde::serialize<nlohmann::json>(j_flatten).dump(4));
    fmt::print("{}\n",serde::serialize<nlohmann::json>(j_none).dump(4));
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


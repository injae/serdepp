#include <serdepp/utility.hpp>
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
                 (&Self::object, "object", flatten))
    std::string type;
    Object object;
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

    auto j_flatten = serde::serialize<std::vector<Test>>(jflat);
    auto j_none = serde::serialize<std::vector<Test>>(j);

    fmt::print("{}\n",serde::deserialize<nlohmann::json>(j_flatten).dump(4));
    fmt::print("{}\n",serde::deserialize<nlohmann::json>(j_none).dump(4));
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

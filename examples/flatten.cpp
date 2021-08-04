#include <serdepp/utility.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/rapidjson.hpp>

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

using namespace rapidjson;
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

    //fmt::print("{}\n",serde::serialize<nlohmann::json>(j_flatten).dump(4));
    //fmt::print("{}\n",serde::serialize<nlohmann::json>(j_none).dump(4));

    //auto doc_flat = serde::serialize<rapidjson::Document>(j_flatten[0]);

    auto print = [](auto& doc) {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);
        std::cout << "doc:" << buffer.GetString() << std::endl;
    };
    auto doc_flat = serde::serialize<rapidjson::Document>(j_flatten);
    print(doc_flat);
    auto doc_none = serde::serialize<rapidjson::Document>(j_none);
    print(doc_none);

    fmt::print("{}\n",serde::deserialize<std::vector<Test>>(doc_flat));
    fmt::print("{}\n",serde::deserialize<std::vector<Test>>(doc_none));

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

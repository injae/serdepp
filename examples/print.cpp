
#include <serdepp/serde.hpp>
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
                 [attributes(flatten)]
                 (&Self::object, "object"))
    std::string type;
    Object object;
};

int main(int argc, char *argv[])
{
    nlohmann::json jflat = R"([
    {"type": "circle", "radius": 5},
    {"type": "rectangle", "width": 6, "height": 5}
    ])"_json;

    nlohmann::json j = R"([
    {"type": "circle", "object": {"radius" : 5}},
    {"type": "rectangle", "object": {"width": 6, "height": 5}}
    ])"_json;

    auto j_flatten = serde::deserialize<std::vector<Test>>(jflat);

    std::cout << serde::to_string(j_flatten) << "\n";
    
    return 0;
}

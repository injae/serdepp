
#include <serdepp/serde.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

template<class T>
auto test_print(T& value) {
    using namespace serde;
    fmt::print("to string: {}\n", to_string(value));
    auto json = serialize<nlohmann::json>(value);
    fmt::print("to adaptor: {}\n", json.dump(4));
    T pt = deserialize<T>(json);
    fmt::print("from adaptor: {}\n", to_string(pt));
    if constexpr(std::is_pointer_v<T>) { delete pt; }
}

int main(int argc, char *argv[])
{
    //static_assert(serde::is_pointer_v<int*>);
    static_assert(serde::is_pointer_v<std::unique_ptr<int>>);
    static_assert(serde::is_pointer_v<std::shared_ptr<int>>);

    //std::unique_ptr<int> i = serde::is_pointer<std::unique_ptr<int>>::null_type;
    std::unique_ptr<int> i = serde::meta::is_pointer<std::unique_ptr<int>>::init();
    *i = 2;
        
    int* j = new int();
    *j = 1;

    std::shared_ptr<int> k = serde::meta::is_pointer<std::shared_ptr<int>>::init();
    *k = 3;

    test_print(i);
    test_print(k);
    test_print(j);


    delete j;

    return 0;
}

#include <serdepp/serde.hpp>
#include <serdepp/adaptor/reflection.hpp>

struct A {
    DERIVE_SERDE(A,_SF_(a)_SF_(b)_SF_(c)_SF_(d)_SF_(e))
    int a;
    std::string b;
    double c;
    std::vector<int> d;
    int e;
};

int main(int argc, char* argv[]) {
    constexpr auto info = serde::type_info<A>;

    static_assert(serde::type_info<A>.size == 5);
    static_assert(serde::tuple_size_v<A> == 5);
    static_assert(std::is_same_v<serde::to_tuple_t<A>, std::tuple<int, std::string, double, std::vector<int>, int>>);
    static_assert(std::is_same_v<int, std::tuple_element_t<0, serde::to_tuple_t<A>>>);

    auto a = A{1, "hello", 3., {1,2}, 2};

    auto to_tuple = serde::make_tuple(a);

    std::string& member_a = info.member<1>(a);
    member_a = "why";

    double& member_b_info = info.member<double>(a, "c");
    member_b_info = 3.14;

    auto member_d_info = info.member_info<3>(a);
    [[maybe_unused]] std::string_view member_d_name = member_d_info.name();
    [[maybe_unused]] std::vector<int>& member_d = member_d_info.value();

    auto names = info.member_names();
    for(auto& name : names.members()) {
        std::cout << name << "\n";
    }

    return 0;
}

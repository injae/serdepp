
#include <serdepp/serde.hpp>
#include <serdepp/adaptor/reflection.hpp>

struct A {
    DERIVE_SERDE(A, _SF_(a)_SF_(b)_SF_(c))
    int a;
    std::string b;
    double c;
};

int main(int argc, char* argv[]) {
    auto a = A{1, "hello", 3.};
    const auto info = serde::type_info<A>();
    std::cout << info << "\n";
    auto member = info.member<std::string>(a, 1);
    std::cout << member << "\n";
    member.value() = "why";
    std::cout << member << "\n";
    auto names = info.member_names();
    for(auto& name : names.names()) {
        std::cout << name << "\n";
    }

    return 0;
}

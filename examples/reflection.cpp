#include <serdepp/serde.hpp>
#include <serdepp/adaptor/reflection.hpp>

struct Test {
    DERIVE_SERDE(Test,
            (&Self::test, "test")
            (&Self::str, "str"))
    int test;
    std::string str;
};

int main(int argc, char *argv[])
{
    auto t = Test{1,"hello"};
    std:: cout << t << "\n";
    serde::mem_get<std::string>(t, "str") = "why";
    serde::mem_get<int, 0>(t) = 200;
    std:: cout << t << "\n";

    return 0;
}


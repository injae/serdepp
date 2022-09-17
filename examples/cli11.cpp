#include <serdepp/serde.hpp>
#include <serdepp/adaptor/cli11.hpp>
#include <serdepp/attribute/cli11.hpp>
#include <serdepp/adaptor/fmt.hpp>

struct callback_interface {
    static constexpr std::string_view desc = "";
    static void callback(std::vector<std::string>& args){ throw serde::unimplemented_error("need callback function"); }
};

using namespace serde::attribute::cli11;
template<typename T=callback_interface>
struct SingleCommand {
    SingleCommand()=default;
    DERIVE_SERDE(SingleCommand<T>,
                 .attrs(desc{std::string{T::desc}}, cli11::callback{SingleCommand<T>::callback})
                 [attrs(cli11::option("args", "args"))]_SF_(args))
    std::vector<std::string> args;
    static void callback(SingleCommand<T>& cmd) { T::callback(cmd.args); }
};

struct Test : callback_interface {
    static constexpr std::string_view desc ="Test1 Descripition";
    static void callback(std::vector<std::string>& args) {
        fmt::print("Test {}\n",args);

        throw CLI::Success{};
    }
};

struct Test2 : callback_interface {
    static constexpr std::string_view desc ="Test2 Descripition";
    static void callback(std::vector<std::string>& args) {
        fmt::print("Test2 {}\n",args);

        throw CLI::Success{};
    }
};

struct Nested {
    DERIVE_SERDE(Nested,
                 .attrs(desc{"Nested Cmd"}, callback{Nested::execute},
                        option_group{"Type", "Project Type", [](auto* g){ g->require_option(1); }},
                        sub_command{"test2", "Test2 subcommand", Nested::test2},
                        sub_command{"test1", "Test1 subcommand", Nested::test1}
                        )
                 [attrs(flag("--check,-c", "on off"))]_SF_(check)
                 [attrs(option("--hello", "hello"))]_SF_(hello)
                 [attrs(group_flag("Type","-l,--lib", "lib"))]_SF_(lib)
                 [attrs(group_flag("Type","-b,--bin", "bin"))]_SF_(bin)
                 [attrs(option("first", "first"))]_SF_(first)
                 [attrs(option("second", "second"))]_SF_(second)
                 _SF_(test3))

    static void execute(Nested& n) {
        fmt::print("{}\n",n);
        throw CLI::Success{};
    }

    static void test2(Nested& n) {
        fmt::print("hello2 {}", n);
    }

    static void test1(Nested& n) {
        fmt::print("hello {}", n);
    }

    bool check;
    bool lib;
    bool bin;
    std::string hello;
    std::string first;
    std::string second;
    SingleCommand<Test> test3;
};

struct Cmd {
    DERIVE_SERDE(Cmd,
                 .attrs(command{"Cmd Test"}, callback{Cmd::execute})
                 [attrs(flag("-c{check},-f{fheck}", "on off"))]_SF_(check)
                 [attrs(option("--hello", "hello"))]_SF_(hello)
                 [attrs(option("-D", "CMake option"))]_SF_(cmake)
                 _SF_(nest)
                 _SF_(test)
                 _SF_(t2)
                 )


    static void execute(Cmd& n) {
        fmt::print("{}\n",n);
        throw CLI::Success{};
    }

    std::string check;
    std::string hello;
    std::string cmake;
    SingleCommand<Test> test;
    SingleCommand<Test2> t2;
    Nested nest;
};


int main(int argc, char *argv[]) {
    return serde::cli11_parse<Cmd>(argc, argv);
}

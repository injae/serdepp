#include <serdepp/serde.hpp>
#include <serdepp/adaptor/cli11.hpp>
#include <serdepp/attribute/cli11.hpp>
#include <serdepp/adaptor/fmt.hpp>

struct callback_interface {
    static void callback(std::vector<std::string>& args){ throw serde::unimplemented_error("need callback function"); };
};

using namespace serde::attribute::cli11;
template<typename T=callback_interface>
struct SingleCommand {
    SingleCommand()=default;
    DERIVE_SERDE(SingleCommand<T>, .attributes(cli11::callback{SingleCommand<T>::callback})
                    [attrs(cli11::option("args", "args"))]_SF_(args))
    std::vector<std::string> args;

    static void callback(SingleCommand<T>& cmd) { T::callback(cmd.args); }
};

struct Test : callback_interface {
    static void callback(std::vector<std::string>& args) {
        fmt::print("Test {}\n",args);

        throw CLI::Success{};
    }
};

struct Test2 : callback_interface {
    static void callback(std::vector<std::string>& args) {
        fmt::print("Test2 {}\n",args);

        throw CLI::Success{};
    }
};


struct Nested {
    DERIVE_SERDE(Nested, .attributes(desc{"Nested Cmd"}, callback{Nested::execute})
                 [attrs(flag("--check,-c", "on off", [](auto* opt){ opt->required(); }))]_SF_(check)
                 [attrs(option("--hello", "hello"))]_SF_(hello)
                 [attrs(option("first", "first", [](auto* opt){ opt->required(); }))]_SF_(first)
                 [attrs(option("second", "second"))]_SF_(second)
                 _SF_(test))

    static void execute(Nested& n) {
        fmt::print("{}\n",n);

        throw CLI::Success{};
    }

    bool check;
    std::string hello;
    std::string first;
    std::string second;
    SingleCommand<Test> test;
};

struct Cmd {
    DERIVE_SERDE(Cmd,
                 .attrs(command{"Cmd Test"},
                        option_group{"Type", "Project Type", [](auto* g){ g->require_option(1); }},
                        callback{Cmd::execute})
                 [attrs(flag("-c{check},-f{fheck}", "on off"))]_SF_(check)
                 [attrs(option("--hello", "hello"))]_SF_(hello)
                 [attrs(option("-D", "CMake option"))]_SF_(cmake)
                 [attrs(group_flag("Type","-l,--lib", "lib"))]_SF_(lib)
                 [attrs(group_flag("Type","-b,--bin", "bin"))]_SF_(bin)
                 _SF_(nest)
                 _SF_(test)
                 _SF_(t2)
                 )


    static void execute(Cmd& n) {
        fmt::print("{}\n",n);
        throw CLI::Success{};
    }

    std::string check;
    bool lib;
    bool bin;
    std::string hello;
    std::string cmake;
    SingleCommand<Test> test;
    SingleCommand<Test2> t2;
    Nested nest;
};


int main(int argc, char *argv[]) {
    CLI::App app;
    
    Cmd cmd;
    serde::serialize_to(cmd, app);

    CLI11_PARSE(app, argc, argv);
    return 0;
}

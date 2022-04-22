#include <serdepp/serde.hpp>
#include <serdepp/adaptor/cli11.hpp>
#include <serdepp/attribute/cli11.hpp>
#include <serdepp/adaptor/fmt.hpp>

struct Nested {
    DERIVE_SERDE(Nested, .attributes(cli11::desc{"Nested Cmd"}, cli11::callback{Nested::execute})
                 [attrs(cli11::flag("--check,-c", "on off", [](auto* opt){ opt->required(); }))]_SF_(check)
                 [attrs(cli11::option("--hello", "hello"))]_SF_(hello)
                 [attrs(cli11::option("first", "first"))]_SF_(first)
                 [attrs(cli11::option("second", "second"))]_SF_(second)
                 )

    static void execute(Nested& n) {
        fmt::print("{}\n",n);
        exit(true);
    }

    bool check;
    std::string hello;
    std::string first;
    std::string second;
};

struct Cmd {
    DERIVE_SERDE(Cmd, .attributes(cli11::desc{"Cmd Test"}, cli11::callback{Cmd::execute}) 
                 [attrs(cli11::flag("--check,-c", "on off"))]_SF_(check)
                 [attrs(cli11::option("--hello", "hello"))]_SF_(hello)
                 _SF_(nest)
                 )

    static void execute(Cmd& n) {
        fmt::print("{}\n",n);
        exit(true);
    }

    bool check;
    std::string hello;
    Nested nest;
};


int main(int argc, char *argv[])
{
    CLI::App app;
    Cmd cmd;
    serde::serialize_to(cmd, app);
    CLI11_PARSE(app, argc, argv);
    return 0;
}

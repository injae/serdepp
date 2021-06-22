#include <benchmark/benchmark.h>
#include <fmt/format.h>
#include <any>
#include <functional>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <variant>
#include <serdepp/serializer.hpp>
#include <nlohmann/json.hpp>

template<class... Ts> struct attributes : Ts... { using Ts::operator()...; };
template<class... Ts> attributes(Ts...) -> attributes<Ts...>;

template<class T>
struct _serde_context {
    using TYPE = T;
    _serde_context(std::variant<T>& format) : format(format) {}
    std::variant<T>& format;
    T& get() { return std::get<T>(format);}
    template<class OPT> void visit(OPT&& opt) { std::visit(opt, format); }
};

template<class Format, class T, size_t index>
struct serde_struct_member {

};

template<class Format, class T, int size=1>
struct serde_struct {
    constexpr static int size_ = size;
    using TYPE = T;
    using Context = _serde_context<Format>;
    using Func = std::function<void(Context& context, T& value)>;
    template<size_t Index> using mem = serde_struct_member<Format, T, Index>;
    std::pair<int, int> iter_[size_];

    constexpr serde_struct() : iter_() {  }

    //template<class MEM_PTR, class... Ts>
    //constexpr auto& mem(int index, MEM_PTR&& ptr, std::string_view name, attributes<Ts...>&& opt) {
    //    iter_[index] = {
    //        [ptr = std::move(ptr), name = std::move(name), opt = std::move(opt)]
    //        (Context& context, T& value) {
    //            auto& mem_ptr = std::invoke(ptr, value);
    //            if constexpr (serde::is_optional_v<decltype(mem_ptr)>) {
    //                if (mem_ptr.has_value()) context.get()[name] = mem_ptr.value();
    //            } else {

    //            }
    //            context.visit(std::move(opt));
    //        },
    //        [ptr = std::move(ptr), name = std::move(name), opt = std::move(opt)]
    //        (Context& context, T& value){
    //        constexpr auto opt_check = serde::is_optional_v<decltype(std::invoke(ptr, value))>;
    //            if constexpr (opt_check) { value.*ptr = context.get()[name];    }
    //            else                     { value.*ptr = context.get().at(name); }
    //             context.visit(opt);
    //        }};
    //    return *this;
    //}

    //template<class MEM_PTR>
    //constexpr auto& mem(int index, MEM_PTR&& ptr, std::string_view name) {
    //    iter_[index] = {
    //            [ptr = std::move(ptr), name]
    //            (Context& context, T& value) {
    //            auto& mem_ptr = std::invoke(ptr, value);
    //            if constexpr (serde::is_optional_v<decltype(mem_ptr)>) {
    //                if (mem_ptr.has_value()) context.get()[name] = mem_ptr.value();
    //            } else {
    //                context.get()[name] = mem_ptr;
    //            }
    //         },
    //           [ptr = std::move(ptr), name]
    //           (Context& context, T& value){
    //           constexpr auto opt_check = serde::is_optional_v<decltype(std::invoke(ptr, value))>;
    //           if constexpr (opt_check) { value.*ptr = context.get()[name];    }
    //           else                     { value.*ptr = context.get().at(name); }
    //        }};
    //    return *this;

    //}

    void set(Context& context, T& value) {
        for(auto& [getter, setter] : iter_) { setter(context, value); }
    }
    void get(Context& context, T& value) {
        for(auto& [getter, setter] : iter_) { getter(context, value); }
    }
};

//template<class Context, class T> serde_struct() -> serde_struct<Context, T>;

struct test {
    template<class Context>
    constexpr static auto serde() {
        using T = serde_struct<Context, test, 3>;
        constexpr auto ss = T{};
        //ss.mem(0, &test::a, "a", attributes{[](auto& format){
        //    using T = std::decay_t<decltype(format)>;
        //    if constexpr(std::is_same_v<std::map<std::string_view, int>, T>) {

        //    }
        //} });
        //ss.mem(1, &test::b, "b");
        //ss.mem(2, &test::opt, "opt");
        return ss;
    }

    int a=0;
    int b=0;
    std::optional<int> opt;
};

struct TestStruct {
    std::optional<std::string> str;
    int i;
    std::vector<std::string> vec;
};


static void set_bench(benchmark::State& state) {
    std::variant<std::map<std::string_view, int>> v = std::map<std::string_view, int>(); 
    _serde_context context(v);
    auto& m = std::get<std::map<std::string_view, int>>(v);
    m["a"] = 1;
    m["b"] = 2;
    m["opt"] = 3;
    test t;
    constexpr auto ss = test::serde<std::map<std::string_view, int>>();

    for(auto _ : state) {
        //test::serde<std::map<std::string_view, int>>().set(context, t);
        //ss.set(context, t);
    }
}

static void get_bench(benchmark::State& state) {
    std::variant<std::map<std::string_view, int>> v = std::map<std::string_view, int>(); 
    _serde_context context(v);
    //m["a"] = 1;
    //m["b"] = 2;
    //m["opt"] = 3;
    test t;
    t.a = 1;
    t.b = 2;
    t.opt = 3;
    constexpr auto ss = test::serde<std::map<std::string_view, int>>();

    for(auto _ : state) {
        //test::serde<std::map<std::string_view, int>>().get(context, t);
        //ss.get(context, t);
    }
}

BENCHMARK(set_bench);
BENCHMARK(get_bench);
BENCHMARK_MAIN();

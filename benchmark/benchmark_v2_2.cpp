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

template<class T, bool is_serialize_=true>
struct _serde_context {
    using Format = T;
    constexpr static bool is_serialize = is_serialize_;
    _serde_context(std::variant<T>& format) : format(format) {}
    std::variant<T>& format;
    T& get() { return std::get<T>(format);}
    template<class OPT> void visit(OPT&& opt) { std::visit(opt, format); }
};

template<class Context, class T>
struct serde_struct {
    using TYPE = T;
    Context& context_;
    T& value_;
    const size_t size_;
    constexpr serde_struct(Context& context, T& value, int size) : context_(context), value_(value), size_(size) {  }

    template<class MEM_PTR, class... Ts>
    inline constexpr serde_struct& field(MEM_PTR&& ptr, std::string_view name, attributes<Ts...>&& opt) {
        if constexpr(Context::is_serialize) {
            constexpr auto opt_check = serde::is_optional_v<decltype(std::invoke(ptr, value_))>;
            if constexpr (opt_check) { value_.*ptr = context_.get()[name];    }
            else                     { value_.*ptr = context_.get().at(name); }
            context_.visit(opt);
        } else {
            auto& mem_ptr = std::invoke(ptr, value_);
            if constexpr (serde::is_optional_v<decltype(mem_ptr)>) {
                if (mem_ptr.has_value()) context_.get()[name] = mem_ptr.value();
            } else {
                context_.get()[name] = mem_ptr;
            }
            context_.visit(std::move(opt));
        }
        return *this;
    }

    template<class MEM_PTR>
    inline constexpr serde_struct& field(MEM_PTR&& ptr, std::string_view name) {
        if constexpr(Context::is_serialize) {
            constexpr auto opt_check = serde::is_optional_v<decltype(std::invoke(ptr, value_))>;
            if constexpr (opt_check) { value_.*ptr = context_.get()[name];    }
            else                     { value_.*ptr = context_.get().at(name); }
        } else {
            auto& mem_ptr = std::invoke(ptr, value_);
            if constexpr (serde::is_optional_v<decltype(mem_ptr)>) {
                if (mem_ptr.has_value()) context_.get()[name] = mem_ptr.value();
            } else {
                context_.get()[name] = mem_ptr;
            }
        }
        return *this;
    }
};

template<class Context, class T> serde_struct(Context, T) -> serde_struct<Context, T>;

struct test {
    template<class Context>
    static constexpr auto serde(Context& context, test& value) {
      return serde_struct<Context, test>(context, value, 3)
          .field(&test::a, "a",
                 attributes{
                     //[](std::map<std::string_view, int> &format) {fmt::print("hello");},
                     //[](auto &format) {fmt::print("oooooo");}
                     [](auto& format) {
                        using T = std::decay_t<decltype(format)>;
                        if constexpr(std::is_same_v<std::map<std::string_view, int>, T>) {}
                     }
                 })
          .field(&test::b, "b")
          .field(&test::opt, "opt");
    }
    int a=0;
    int b=0;
    std::optional<int> opt;
};

static void set_bench(benchmark::State& state) {
    std::variant<std::map<std::string_view, int>> v = std::map<std::string_view, int>(); 
    _serde_context context(v);
    auto& m = std::get<std::map<std::string_view, int>>(v);
    m["a"] = 1;
    m["b"] = 2;
    m["opt"] = 3;
    test t;
    auto ss = test::serde(context, t);

    for(auto _ : state) {
        test::serde(context, t);
    }
}

static void get_bench(benchmark::State& state) {
    std::variant<std::map<std::string_view, int>> v = std::map<std::string_view, int>(); 
    _serde_context<std::map<std::string_view, int>, false> context(v);
    //m["a"] = 1;
    //m["b"] = 2;
    //m["opt"] = 3;
    test t;
    t.a = 1;
    t.b = 2;
    t.opt = 3;


    for(auto _ : state) {
        test::serde(context, t);
    }
}

BENCHMARK(set_bench);
BENCHMARK(get_bench);
BENCHMARK_MAIN();

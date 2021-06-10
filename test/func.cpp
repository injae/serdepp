#include <fmt/format.h>
#include <any>
#include <functional>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <serdepp/serializer.hpp>

template<class Context, class T, int size=1>
struct serde_struct {
    constexpr static int size_ = size;
    Context& context_;
    T& value_;
    std::vector<std::pair<std::function<void()>,std::function<void()>>> iter_;

    serde_struct(Context& context, T& value) : context_(context), value_(value) {}

    template<class MEM_PTR>
    constexpr serde_struct& mem(MEM_PTR&& ptr, std::string_view name) {
        constexpr auto opt_check = serde::is_optional_v<decltype(std::invoke(ptr, value_))>;
        iter_.push_back({
           [this, ptr, name] {
                if constexpr (opt_check) {
                    auto& mem_ptr = std::invoke(ptr, value_);
                    if (mem_ptr.has_value()) context_[name] = mem_ptr.value();
                } else {
                    context_[name] = std::invoke(ptr, value_);
                }
           },
           [this, ptr, name] {
                if constexpr (opt_check) { value_.*ptr = context_[name];    }
                else                     { value_.*ptr = context_.at(name); }
           }});
      return *this;
    }

    void set() { for(auto& [getter, setter] : iter_) { setter(); } }
    void get() { for(auto& [getter, setter] : iter_) { getter(); } }
};

template<class Context, class T> serde_struct(Context&, T&) -> serde_struct<Context, T>;

struct test {
    int a=0;
    int b=0;
    std::optional<int> opt;
};

int main(int argc, char* argv[]) {
    std::map<std::string_view, int> m;
    m["a"] = 1;
    m["b"] = 2;
    m["opt"] = 3;
    test t;
    //t.a = 1;
    //t.b = 2;
    //t.opt = 3;
    serde_struct ss(m, t);
    ss.mem(&test::a, "a");
    ss.mem(&test::b, "b");
    ss.mem(&test::opt, "opt");
    ss.set();
    //ss.get();

    fmt::print("{}", t.a);
    fmt::print("{}", t.b);
    fmt::print("{}", t.opt.value());

    // ss.iter_[0].first();
    return 0;
}





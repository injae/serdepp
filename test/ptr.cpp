
#include <fmt/format.h>
#include <any>
#include <functional>
#include <map>
#include <nameof.hpp>
#include <optional>
#include <serdepp/serializer.hpp>


template<typename T, int num>
struct mem {
    std::any var;
    template<class U>
    mem(U&& mem_ptr) { var = &mem_ptr; }

    template<typename Z, typename V>
    void set(T& target, V& v) { return target.*(std::any_cast<Z>(var)) = v; }
};


template<class Context, class T, int size=1>
struct serde_struct {
    constexpr static int size_ = size;
    Context& context_;
    T& value_;
    serde_struct(Context& context, T& value) : context_(context), value_(value) {
      
    }
    template<typename MEM>
    serde_struct& mem(MEM&& ptr, std::string_view name) {
        using namespace serde;
        using mem_type = decltype(value_.*(ptr));
        //if constexpr (context_.is_serialize_step()) {
        if constexpr (serde::is_optionable_v<mem_type>) {
            value_.*(ptr) = std::any_cast<type::is_optional_element<std::remove_reference_t<mem_type>>>(context_[name]);
        } else {
            value_.*(ptr) = std::any_cast<decltype(value_.*(ptr))>(context_[name]);
        }

        //} else {
        //    context_[name] = value_.*(ptr);
        //}
        return *this;
    }
};

template<class Context, class T> serde_struct(Context&, T&) -> serde_struct<Context, T>;

#define MEM(var) mem(var, NAMEOF(var))


struct test {
    int a=0;
    int b=0;
    std::optional<int> opt;
};

int main(int argc, char* argv[]) {
    std::any mem_a = &test::a;
    std::map<std::string_view, std::any> m;
    m["a"] = 1; 
    m["b"] = 2; 

    test t;

    serde_struct<10>(m, t)
        .field("a", &test.a)
        .field("b", &test.b)
        .field("c", &test.opt)
        .end();

    serde_struct<decltype(m), decltype(t), 10>(m, t)
        .MEM(&test::a)
        .MEM(&test::b)
        .MEM(&test::opt)
        ;

    //t.*(std::any_cast<decltype(&test::a)>(mem_a)) = 1;

    fmt::print("-{}",t.a);
    fmt::print("-{}",t.b);
    
    return 0;
}



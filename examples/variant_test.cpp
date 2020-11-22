
#include "serdepp/utility.hpp"
#include "serdepp/adaptor/toml11.hpp"
#include <variant>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct te {
    template<typename T>
    constexpr static bool is() {
        return serde::meta::is_literal<T>();
    }
};

template<typename T, int... Is>
constexpr int get_type_index_impl(T&& t, std::integer_sequence<int, Is...>) {
    using O = std::remove_cv_t<T>;
    std::array<bool, sizeof...(Is)> buf = { (te::is<typename std::variant_alternative_t<Is, O>>, 0)... };
    for(int i =0; i < buf.size(); ++i) { if(buf[i]) return i; }
}

template<typename... Ts>
constexpr int get_type_index(std::variant<Ts...>& t) {
    return get_type_index_impl(t, std::make_integer_sequence<int,sizeof...(Ts)>());
}

#include "serdepp/adaptor/toml11.hpp"

struct test {
    derive_serde(test, ctx
                 .name(name)
                 .or_value(z, "z")
                 .tag(y, "y", "hello")
                 .no_remain();
                 )
    std::string z;
    std::string name;
    std::optional<std::string> y;
};

struct p_test {
    derive_serde(p_test, ctx
                 .TAG(m)
                 .TAG(z);
                 )
    std::map<std::string, test> m;
    std::vector<test> z;
};

int main(int argc, char* argv[]) {
    std::string t_data = R"(
      [m]
        x = "1"
        h = { z = "h", y = "hi"}

      [[z]]
        z = "not" 

      [[z]]
        z = "why" 
    )";
    std::istringstream t_stream(t_data);
    auto t_t = toml::parse(t_stream);
    auto t_xx = serde::serialize<p_test>(t_t, "p_test");
    fmt::print("{}\n",t_xx);
    auto t_yy = serde::deserialize<toml::value>(t_xx);
    fmt::print("{}\n",t_yy);
    //std::variant<std::string_view, int, ttt> v;
    //nlohmann::json s;
    //    fmt::print("index:{}",get_type_index(v));

    // call tag<variant>(v, key)
    //std::visit([&s](auto&& type) {
    //    using namespace serde;
    //    using T = std::decay_t<decltype(type)>;
    //    if constexpr(meta::is_literal<T>::value) {
    //        fmt::print("is literal: {}\n",type);
    //    } else {
    //        fmt::print("{}\n",type);
    //    }
    //}, v);

    return 0;
}


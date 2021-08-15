#include <benchmark/benchmark.h>
#include <serdepp/serde.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

namespace serde{
    template<class Context, class T>
    class serde_struct_v2 {
        using TYPE = T;
        Context& context_;
        T& value_;
        constexpr const static std::string_view type = nameof::nameof_type<T>();
    public:
        constexpr serde_struct_v2(Context& context, T& value) : context_(context), value_(value) {}

        template<class MEM_PTR, typename Attribute, typename... Attributes>
        inline constexpr serde_struct_v2& field(MEM_PTR&& ptr, std::string_view name,


                                             Attribute&& attribute, Attributes&&... attributes) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(!Context::is_serialize) {
                attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::detail::serializer_call_attr{});
            } else {
                attribute.template into<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::detail::serializer_call_attr{});
            }
            return *this;
        }

        template<class MEM_PTR>
        inline constexpr serde_struct_v2& field(MEM_PTR&& ptr, std::string_view name) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(!Context::is_serialize) {
                serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
            } else {
                serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
            }
            return *this;
        }
        
        inline constexpr serde_struct_v2& no_remain() {
            if constexpr (!Context::is_serialize) {
                const auto adaptor_size = Context::Helper::is_struct(context_.adaptor)
                    ? Context::Helper::size(context_.adaptor)
                    : 1;
                const auto serde_size   = context_.read_count_;
                if(adaptor_size > serde_size) {
                    throw unregisted_data_error(fmt::format(
                                                    "serde[{}] read:{} != adaptor[{}] read:{}",
                                                    type,
                                                    serde_size,
                                                    nameof::nameof_short_type<decltype(context_.adaptor)>(),
                                                    adaptor_size));
                }
            }
            return *this;
        }


        template<class MEM_PTR>
        inline constexpr serde_struct_v2& operator()(MEM_PTR&& ptr, std::string_view name) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(!Context::is_serialize) {
                serde::serde_serializer<rtype, Context>::from(context_, value_.*ptr, name);
            } else {
                serde::serde_serializer<rtype, Context>::into(context_, value_.*ptr, name);
            }
            return *this;
        }

        template<class MEM_PTR, typename Attribute, typename... Attributes>
        inline constexpr serde_struct_v2& operator()(MEM_PTR&& ptr, std::string_view name,
                                                  Attribute&& attribute, Attributes&&... attributes) {
            using rtype = std::remove_reference_t<decltype(std::invoke(ptr, value_))>;
            if constexpr(!Context::is_serialize) {
                attribute.template from<rtype, Context>(context_, value_.*ptr, name,
                                                        std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::serializer_call);
            } else {
                attribute.template into<rtype, Context>(context_, value_.*ptr, name,
       
                                                 std::forward<meta::remove_cvref_t<Attributes>>
                                                        (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                        attribute::serializer_call);
            }
            return *this;
        }

        template<typename Attribute, typename... Attributes>
        inline constexpr serde_struct_v2& attr(Attribute&& attribute, Attributes&&... attributes) {
            if constexpr(!Context::is_serialize) {
                attribute.template from<T, Context>(context_, value_, "",
                                                    std::forward<meta::remove_cvref_t<Attributes>>
                                                    (const_cast<meta::remove_cvref_t<Attributes>&>(attributes))...,
                                                    attribute::pass);
            } else {
              attribute.template into<T, Context>(
                  context_, value_, "",
                  std::forward<meta::remove_cvref_t<Attributes>>(
                      const_cast<meta::remove_cvref_t<Attributes> &>(
                          attributes))...,
                  attribute::pass);
            }
            return *this;
        }
        template <typename... Attriss>
        struct Attribute {
            using def = serde_struct_v2<Context, T>;
            Attribute(def& s, std::tuple<Attriss...>&& attr) : s(s), attributes(std::move(attr)) {}
            template<class MEM_PTR>
            inline constexpr def& operator()(MEM_PTR&& ptr, std::string_view name){
                auto ptsr = [&](auto... v) {s(ptr, name, v...);};
                std::apply(ptsr, attributes);
                return s;
            }
            template <typename... Attrisss>
            constexpr inline def&  operator[](std::tuple<Attrisss...>&& attr) {
                auto ptsr = [&](auto... v) {s.attr(v...);};
                std::apply(ptsr, attributes);
                return s;
            }

            def& s;
            std::tuple<Attriss...> attributes;
        };

        template <typename... Attriss>
        constexpr inline Attribute<Attriss...> operator[](std::tuple<Attriss...>&& attr) {
            return Attribute<Attriss...>(*this, std::move(attr));
        }

    };
    template <class Context, class T> serde_struct_v2(Context &, T &) -> serde_struct_v2<Context, T>;

    template<typename... Ty>
    inline constexpr std::tuple<Ty...> attributes(Ty... arg) { return std::make_tuple(arg...);}
}

struct Object {
    DERIVE_SERDE(Object,
                 (&Self::radius, "radius")
                 (&Self::width, "width")
                 (&Self::height, "height"))
    std::optional<int> radius;
    std::optional<int> width;
    std::optional<int> height;
};
struct Test {
    DERIVE_SERDE(Test,
                 (&Self::type, "type")
                  (&Self::object, "object", flatten)
                 )
    std::string type;
    Object object;
};

struct Test_v2 {
    template<class Context> 
    constexpr static void serde(Context& context, Test_v2& value) {
        using namespace serde::attribute;    
        using serde::attributes;
        using Self = Test_v2;
        serde::serde_struct_v2(context, value)
            (&Self::type, "type")
            [attributes(flatten)]
            (&Self::object, "object");
    }
    std::string type;
    Object object;
};

nlohmann::json jflat = R"([{"type": "circle", "radius": 5}, {"type": "rectangle", "width": 6, "height": 5}])"_json;

nlohmann::json j = R"([{"type": "circle", "object": {"radius" : 5}},
 {"type": "rectangle", "object": {"width": 6, "height": 5}}])"_json;

static void set_with_sugar_bench(benchmark::State& state) {
    auto test_data = jflat;
    for(auto _ : state) {
        serde::deserialize<std::vector<Test>>(test_data);
    }
}

static void set_with_default_bench(benchmark::State& state) {
    auto test_data = jflat;
    for(auto _ : state) {
        serde::deserialize<std::vector<Test_v2>>(test_data);
    }
}

BENCHMARK(set_with_default_bench);
BENCHMARK(set_with_sugar_bench);
BENCHMARK_MAIN();

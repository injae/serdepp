#include "serdepp/serde.hpp"
#include "serdepp/adaptor/fmt.hpp"

namespace serde::attribute {
    namespace detail {
        struct tttt {
            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                       Next&& next_attr, Attributes&& ...remains) const{
                fmt::print("from\n");
                next_attr.from(ctx, data, key, remains...);
            }

            template <typename T, typename serde_ctx, typename Next, typename... Attributes>
            constexpr inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                       Next&& next_attr, Attributes&& ...remains) const{
                fmt::print("into\n");
                next_attr.into(ctx, data, key, remains...);
            }
        };
    }
    [[maybe_unused]] constexpr static auto tttt = detail::tttt{};
}

struct Test {
    DERIVE_SERDE(Test,
            [attributes(flatten)]
            (&Self::test, "test")
            [attributes(flatten, default_(""))]
            (&Self::str, "str"))
    int test;
    std::string str;
};

int main(int argc, char *argv[])
{
    std::cout << Test{1} << "\n";
    
    return 0;
}

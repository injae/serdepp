#include <serdepp/utility.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

enum class t_enum { A, B };

struct example {
    DERIVE_SERDE(example,
                 (&Self::number_,  "number")
                 (&Self::vec_,     "vec",     skip_de{}) //skip deserialize
                 (&Self::opt_vec_, "opt_vec")
                 (&Self::tenum_,   "t_enum"))

    int number_;
    std::vector<std::string> vec_;
    std::optional<std::vector<std::string>> opt_vec_;
    t_enum tenum_;
};  

int main() {
    example ex;
    ex.number_ = 1024;
    ex.vec_ = {"a", "b", "c"};
    ex.tenum_ = t_enum::B;

    nlohmann::json json_from_ex = serde::deserialize<nlohmann::json>(ex);
    fmt::print("\n---\n");
    fmt::print("json:{}\n",json_from_ex.dump(4));

    example ex_from_json = serde::serialize<example>(json_from_ex);
    fmt::print("\n---\n");

    fmt::print("json:{}\n",json_from_ex.dump(4));

    fmt::print("\n---\n");
    fmt::print("fmt:{}\n",ex_from_json);
}

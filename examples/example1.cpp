#include <serdepp/utility.hpp>
#include <serdepp/adaptor/nlohmann_json.hpp>

enum class t_enum { A, B };

struct example {
    DERIVE_SERDE(example,
                 (&Self::number_,  "number", skip) //skip
                 (&Self::vec_,     "vec") 
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

    nlohmann::json json_from_ex = serde::serialize<nlohmann::json>(ex);
    example ex_from_json = serde::deserialize<example>(json_from_ex);
    fmt::print("json:{}\n",json_from_ex.dump(4));
    fmt::print("fmt:{}\n",ex_from_json);
}

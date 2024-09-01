#include <string>

enum class token_type {
  tok_number,
  tok_symbol,
  tok_left_paren,
  tok_right_paren,
  tok_end_of_file
};

class token {
 public:
  token(token_type type, const std::string &value)
      : type_(type), value_(value) {}
  token_type get_type() const;
  std::string get_value() const;

 private:
  token_type type_;
  std::string value_;
};

#include "./lexer.h"

std::vector<token> tokenize(const std::string &source) {
  lexer lex(source);
  std::vector<token> tokens;

  token tok = lex.next_token();

  while (tok.get_type() != token_type::token_end_of_file) {
    tokens.push_back(tok);
    tok = lex.next_token();
  }

  return tokens;
}

token_type token::get_type() const { return type_; }

std::string token::get_value() const { return value_; }

lexer::lexer(const std::string &source) : source_(source), current_pos_(0) {
  eat();
}

token lexer::next_token() {
  skip_whitespace();

  if (current_pos_ >= source_.size()) {
    return token(token_type::token_end_of_file, "");
  }

  char curr = current_char();

  if (curr == '(') {
    eat();
    return token(token_type::token_left_paren, "(");
  } else if (curr == ')') {
    eat();
    return token(token_type::token_right_paren, ")");
  } else if (std::isdigit(curr)) {
    return number();
  } else if (std::isalpha(curr) || curr == '_') {
    return symbol();
  } else {
    throw std::runtime_error("unexpected character: " + std::string(1, curr));
  }
}

char lexer::current_char() const { return source_[current_pos_]; }

void lexer::eat() {
  if (current_pos_ < source_.size()) {
    ++current_pos_;
  }
}

void lexer::skip_whitespace() {
  while (current_pos_ < source_.size() && std::isspace(current_char())) {
    eat();
  }
}

token lexer::number() {
  std::size_t start_pos = current_pos_;

  while (current_pos_ < source_.size() && std::isdigit(current_char())) {
    eat();
  }

  return token(token_type::token_number,
               source_.substr(start_pos, current_pos_ - start_pos));
}

token lexer::symbol() {
  std::size_t start_pos = current_pos_;

  while (current_pos_ < source_.size() &&
         (std::isalnum(current_char()) || current_char() == '_')) {
    eat();
  }

  return token(token_type::token_symbol,
               source_.substr(start_pos, current_pos_ - start_pos));
}

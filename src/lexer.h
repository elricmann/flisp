#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

enum class token_type {
  token_integer,
  token_float,
  token_boolean,
  token_string_literal,
  token_symbol,
  token_left_paren,
  token_right_paren,
  token_end_of_file
};

class token {
 public:
  token(token_type type, const std::string& value)
      : type_(type), value_(value) {}
  token_type get_type() const;
  std::string get_value() const;

 private:
  token_type type_;
  std::string value_;
};

class lexer {
 public:
  explicit lexer(const std::string& source);
  token next_token();

 private:
  std::string source_;
  std::size_t current_pos_;

  void eat();
  char peek_char() const;
  char current_char() const;
  void skip_whitespace();

  token symbol();
  token string_literal();
  token boolean();
  token number();
};

std::vector<token> tokenize(const std::string& source);
std::ostream& operator<<(std::ostream& os, token_type type);

#endif  // LEXER_H

#include "parser.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <unordered_map>

parser::parser(const std::vector<token>& tokens)
    : tokens_(tokens), current_pos_(0) {}

std::shared_ptr<expr> parser::parse() { return parse_expr(); }

std::shared_ptr<expr> parser::parse_expr() {
  if (match(token_type::token_left_paren)) {
    return parse_list();
  } else {
    return parse_atom();
  }
}

std::shared_ptr<expr> parser::parse_list() {
  eat();  // eat '('
  auto list = std::make_shared<list_expr>();
  while (!match(token_type::token_right_paren) &&
         current_pos_ < tokens_.size()) {
    list->add_expr(parse_expr());
  }
  if (!match(token_type::token_right_paren)) {
    throw std::runtime_error("expected ')'");
  }
  eat();  // eat ')'
  return list;
}

std::shared_ptr<expr> parser::parse_atom() {
  auto tok = current_token();
  eat();
  switch (tok.get_type()) {
    case token_type::token_symbol:
      return std::make_shared<symbol_expr>(tok.get_value());
    case token_type::token_integer:
      return std::make_shared<number_expr>(tok.get_value());
    case token_type::token_float:
      return std::make_shared<number_expr>(tok.get_value());
    case token_type::token_boolean:
      return std::make_shared<boolean_expr>(tok.get_value() == "#t");
    case token_type::token_string_literal:
      return std::make_shared<string_expr>(tok.get_value());
    default:
      throw std::runtime_error("unexpected token: " + tok.get_value());
  }
}

token parser::current_token() const { return tokens_[current_pos_]; }

void parser::eat() {
  if (current_pos_ < tokens_.size()) {
    current_pos_++;
  }
}

bool parser::match(token_type type) const {
  return current_token().get_type() == type;
}

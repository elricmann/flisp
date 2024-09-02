#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex> /*std::type_index*/
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>

#include "lexer.h"

using expr_value = std::variant<int, float, bool, std::string>;

class expr {
 public:
  virtual ~expr() = default;
};

class symbol_expr : public expr {
 public:
  explicit symbol_expr(const std::string& name) : name_(name) {}
  std::string get_name() const { return name_; }

 private:
  std::string name_;
};

class integer_expr : public expr {
 public:
  explicit integer_expr(const std::string& value) : value_(std::stoi(value)) {}
  int get_value() const { return value_; }

 private:
  int value_;
};

class float_expr : public expr {
 public:
  explicit float_expr(const std::string& value) : value_(std::stof(value)) {}
  float get_value() const { return value_; }

 private:
  float value_;
};

class boolean_expr : public expr {
 public:
  explicit boolean_expr(bool value) : value_(value) {}
  bool get_value() const { return value_; }

 private:
  bool value_;
};

class string_expr : public expr {
 public:
  explicit string_expr(const std::string& value) : value_(value) {}
  std::string get_value() const { return value_; }

 private:
  std::string value_;
};

class list_expr : public expr {
 public:
  void add_expr(std::shared_ptr<expr> expression) {
    exprs_.push_back(expression);
  }

  const std::vector<std::shared_ptr<expr>>& get_exprs() const { return exprs_; }

 private:
  std::vector<std::shared_ptr<expr>> exprs_;
};

class parser {
 public:
  explicit parser(const std::vector<token>& tokens);
  std::shared_ptr<expr> parse();

 private:
  const std::vector<token>& tokens_;
  std::size_t current_pos_;

  std::shared_ptr<expr> parse_expr();
  std::shared_ptr<expr> parse_list();
  std::shared_ptr<expr> parse_atom();

  token current_token() const;

  void eat();
  bool match(token_type type) const;
};

void visit(std::shared_ptr<expr> node,
           const std::unordered_map<std::type_index,
                                    std::function<void(std::shared_ptr<expr>)>>&
               callbacks);

#endif  // PARSER_H

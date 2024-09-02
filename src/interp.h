#pragma once

#ifndef INTERP_H
#define INTERP_H

#include <memory>
#include <string>
#include <unordered_map>

#include "parser.h"

class interp {
 public:
  void eval(const std::shared_ptr<expr>& node);

 private:
  std::unordered_map<std::string, expr_value> vmap;
  bool skip_initial_lst;

  void eval_def(const std::shared_ptr<list_expr>& list);
  void eval_set(const std::shared_ptr<list_expr>& list);
  void eval_debug(const std::shared_ptr<list_expr>& list);
};

// the definitions below should remain recursive with regards
// to get_value_from_expr, binary operations accumulate based
// on their corresponding identity elements (e.g. 0 for additive
// identity or 1 for multiplicative identity)

expr_value eval_add(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap);
expr_value eval_sub(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap);
expr_value eval_mul(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap);
expr_value eval_div(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap);

expr_value eval_if(const std::shared_ptr<list_expr>& list,
                   const std::unordered_map<std::string, expr_value>& vmap);

expr_value get_value_from_expr(
    const std::shared_ptr<expr>& node,
    const std::unordered_map<std::string, expr_value>& vmap);

#endif  // INTERP_H

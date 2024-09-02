#pragma once

#ifndef INTERP_H
#define INTERP_H

#include <memory>
#include <string>
#include <unordered_map>

#include "parser.h"

class eval_context {
 public:
  std::unordered_map<std::string, expr_value> vmap;
  std::unordered_map<std::string,
                     std::function<expr_value(std::vector<expr_value>)>>
      fmap;
};

class interp {
 public:
  interp() : ctx() {}

  void eval(eval_context& ctx, const std::shared_ptr<expr>& node);

 private:
  eval_context ctx;
  std::unordered_map<std::string, expr_value> vmap;
  bool skip_initial_lst;

  void eval_def(eval_context& ctx, const std::shared_ptr<list_expr>& list);
  void eval_set(eval_context& ctx, const std::shared_ptr<list_expr>& list);
  void eval_debug(eval_context& ctx, const std::shared_ptr<list_expr>& list);
};

// the definitions below should remain recursive with regards
// to get_value_from_expr, binary operations accumulate based
// on their corresponding identity elements (e.g. 0 for additive
// identity or 1 for multiplicative identity)

expr_value eval_if(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_add(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_sub(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_mul(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_div(eval_context& ctx, const std::shared_ptr<list_expr>& list);

expr_value get_value_from_expr(eval_context& ctx,
                               const std::shared_ptr<expr>& node);

#endif  // INTERP_H

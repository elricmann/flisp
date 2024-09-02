#pragma once

#ifndef INTERP_H
#define INTERP_H

#include <memory>
#include <string>
#include <unordered_map>

#include "parser.h"

class callable;
class eval_context;

using expr_value =
    std::variant<int, float, bool, std::string, std::unique_ptr<callable>>;

// we do this to avoid template overloading which requires specific
// instantiations of flisp functions in lieu of using std::unique_ptr
// on a callable interface which is actually realistic

class callable {
 public:
  virtual ~callable() = default;
  virtual expr_value operator()(eval_context& ctx,
                                std::vector<expr_value> args) = 0;
};

template <typename F>
class callable_impl : public callable {
  F func;

 public:
  callable_impl(F&& f) : func(std::forward<F>(f)) {}

  expr_value operator()(eval_context& ctx,
                        std::vector<expr_value> args) override {
    return func(ctx, std::move(args));
  }
};

class eval_context {
 public:
  std::unordered_map<std::string, expr_value> vmap;
  std::unordered_map<std::string, expr_value> fmap;
  // std::unordered_map<std::string, std::unique_ptr<callable>> fmap;
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

expr_value eval_fun(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_if(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_add(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_sub(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_mul(eval_context& ctx, const std::shared_ptr<list_expr>& list);
expr_value eval_div(eval_context& ctx, const std::shared_ptr<list_expr>& list);

expr_value get_value_from_expr(eval_context& ctx,
                               const std::shared_ptr<expr>& node);

#endif  // INTERP_H

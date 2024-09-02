#include "interp.h"

#include <iostream>

expr_value get_value_from_expr(eval_context& ctx,
                               const std::shared_ptr<expr>& node) {
  if (auto int_node = std::dynamic_pointer_cast<integer_expr>(node)) {
    return int_node->get_value();
  } else if (auto float_node = std::dynamic_pointer_cast<float_expr>(node)) {
    return float_node->get_value();
  } else if (auto bool_node = std::dynamic_pointer_cast<boolean_expr>(node)) {
    return bool_node->get_value();
  } else if (auto str_node = std::dynamic_pointer_cast<string_expr>(node)) {
    return str_node->get_value();
  } else if (auto symbol_node = std::dynamic_pointer_cast<symbol_expr>(node)) {
    const std::string& name = symbol_node->get_name();

    if (ctx.vmap.find(name) != ctx.vmap.end()) {
      return std::move(ctx.vmap.at(name));
    } else {
      std::cerr << "error: identifier '" << name << "' not found" << std::endl;
      exit(1);
    }
  } else if (auto list_node = std::dynamic_pointer_cast<list_expr>(node)) {
    auto fst_expr = list_node->get_exprs().front();
    auto symbol = std::dynamic_pointer_cast<symbol_expr>(fst_expr);

    if (symbol) {
      const std::string& name = symbol->get_name();

      if (name == "+") {
        return eval_add(ctx, list_node);
      } else if (name == "-") {
        return eval_sub(ctx, list_node);
      } else if (name == "*") {
        return eval_mul(ctx, list_node);
      } else if (name == "/") {
        return eval_div(ctx, list_node);
      } else if (name == "if") {
        return eval_if(ctx, list_node);
      } else if (ctx.fmap.find(name) != ctx.fmap.end()) {
        std::vector<expr_value> args;

        for (size_t i = 1; i < list_node->get_exprs().size(); ++i) {
          args.push_back(get_value_from_expr(ctx, list_node->get_exprs()[i]));
        }

        auto& func_value = ctx.fmap.at(name);

        if (auto* func_ptr =
                std::get_if<std::unique_ptr<callable>>(&func_value)) {
          if (func_ptr && *func_ptr) {
            return (**func_ptr)(ctx, std::move(args));
          } else {
            std::cerr << "error: callable function is null" << std::endl;
            exit(1);
          }
        } else {
          std::cerr << "error: '" << name << "' is not a callable function"
                    << std::endl;
          exit(1);
        }
      } else {
        std::cerr << "internal error: function '" << name
                  << "' not found in fmap" << std::endl;
        exit(1);
      }
    }
  }

  std::cerr << "error: unknown expression type" << std::endl;
  exit(1);
}

void interp::eval(eval_context& ctx, const std::shared_ptr<expr>& node) {
  if (auto outer_lst = std::dynamic_pointer_cast<list_expr>(node)) {
    // we need this check to ensure that adjacent
    // nodes are not in conflict with nested nodes
    for (auto&& inner_lst : outer_lst->get_exprs()) {
      eval(ctx, inner_lst);
      skip_initial_lst = false;
    }

    skip_initial_lst = true;

    if (skip_initial_lst) {
      auto fst_expr = outer_lst->get_exprs().front();
      auto symbol = std::dynamic_pointer_cast<symbol_expr>(fst_expr);

      if (symbol) {
        const std::string& name = symbol->get_name();

        if (name == "def") {
          eval_def(ctx, outer_lst);
        } else if (name == "debug") {
          eval_debug(ctx, outer_lst);
        } else if (name == "set") {
          eval_set(ctx, outer_lst);
        } else if (name == "fun") {
          eval_fun(ctx, outer_lst);
        } else if (name == "if") {
          eval_if(ctx, outer_lst);
        }
      }
    }
  }
}

// @todo: prevent redefinition & mutable-by-default
void interp::eval_def(eval_context& ctx,
                      const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  auto value_expr = get_value_from_expr(ctx, lst->get_exprs()[2]);

  if (symbol) {
    ctx.vmap[symbol->get_name()] = std::move(value_expr);
  }
}

void interp::eval_set(eval_context& ctx,
                      const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  auto value_expr = get_value_from_expr(ctx, lst->get_exprs()[2]);

  if (symbol) {
    vmap[symbol->get_name()] = std::move(value_expr);
  }
}

void interp::eval_debug(eval_context& ctx,
                        const std::shared_ptr<list_expr>& list) {
  for (size_t i = 1; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(ctx, list->get_exprs()[i]);

    std::visit(
        [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, int>) {
            std::cout << "int: " << arg << std::endl;
          } else if constexpr (std::is_same_v<T, float>) {
            std::cout << "float: " << arg << std::endl;
          } else if constexpr (std::is_same_v<T, bool>) {
            std::cout << "boolean: " << (arg ? "true" : "false") << std::endl;
          } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "string: " << arg << std::endl;
          }
        },
        value);
  }
}

expr_value eval_add(eval_context& ctx, const std::shared_ptr<list_expr>& list) {
  float acc = 0;

  for (size_t i = 1; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(ctx, list->get_exprs()[i]);

    std::visit(
        [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
            acc += arg;
          } else {
            std::cerr << "error: invalid type for add" << std::endl;
            exit(1);
          }
        },
        value);
  }

  return acc;
}

expr_value eval_sub(eval_context& ctx, const std::shared_ptr<list_expr>& list) {
  if (list->get_exprs().size() < 2) {
    std::cerr << "error: at least one operand required for sub" << std::endl;
    exit(1);
  }

  auto fst = get_value_from_expr(ctx, list->get_exprs()[1]);
  float acc = 0;

  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
          acc = arg;
        } else {
          std::cerr << "error: invalid type for sub" << std::endl;
          exit(1);
        }
      },
      fst);

  for (size_t i = 2; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(ctx, list->get_exprs()[i]);

    std::visit(
        [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
            acc -= arg;
          } else {
            std::cerr << "error: invalid type for sub" << std::endl;
            exit(1);
          }
        },
        value);
  }

  return acc;
}

expr_value eval_mul(eval_context& ctx, const std::shared_ptr<list_expr>& list) {
  float acc = 1;

  for (size_t i = 1; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(ctx, list->get_exprs()[i]);

    std::visit(
        [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
            acc *= arg;
          } else {
            std::cerr << "error: invalid type for mul" << std::endl;
            exit(1);
          }
        },
        value);
  }

  return acc;
}

expr_value eval_div(eval_context& ctx, const std::shared_ptr<list_expr>& list) {
  if (list->get_exprs().size() < 2) {
    std::cerr << "error: at least one operand required for div" << std::endl;
    exit(1);
  }

  auto fst = get_value_from_expr(ctx, list->get_exprs()[1]);
  float acc = 0;

  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
          acc = arg;
        } else {
          std::cerr << "error: invalid type for div" << std::endl;
          exit(1);
        }
      },
      fst);

  for (size_t i = 2; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(ctx, list->get_exprs()[i]);

    std::visit(
        [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
            if (arg == 0) {
              std::cerr << "error: div by zero" << std::endl;
              exit(1);
            }

            acc /= arg;
          } else {
            std::cerr << "error: invalid type for div" << std::endl;
            exit(1);
          }
        },
        value);
  }

  return acc;
}

expr_value eval_if(eval_context& ctx, const std::shared_ptr<list_expr>& list) {
  if (list->get_exprs().size() < 2) {
    std::cerr << "error: 'if' expression requires at least a condition and a "
                 "then clause"
              << std::endl;
    exit(1);
  }

  auto condition_value = get_value_from_expr(ctx, list->get_exprs()[1]);

  bool condition = false;

  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, bool>) {
          condition = arg;
        } else {
          std::cerr << "error: 'if' condition must evaluate to a boolean"
                    << std::endl;
          exit(1);
        }
      },
      condition_value);

  if (condition) {
    return get_value_from_expr(ctx, list->get_exprs()[2]);
  }

  if (list->get_exprs().size() > 3) {
    return get_value_from_expr(ctx, list->get_exprs()[3]);
  }

  return {};
}

expr_value eval_fun(eval_context& ctx, const std::shared_ptr<list_expr>& list) {
  if (list->get_exprs().size() < 3) {
    std::cerr
        << "error: 'fun' expression requires a name, parameters, and a body"
        << std::endl;
    exit(1);
  }

  auto name_expr = std::dynamic_pointer_cast<symbol_expr>(list->get_exprs()[1]);
  auto params_expr = std::dynamic_pointer_cast<list_expr>(list->get_exprs()[2]);
  auto body_expr = std::dynamic_pointer_cast<list_expr>(list->get_exprs()[3]);

  if (!name_expr || !params_expr || !body_expr) {
    std::cerr << "error: invalid 'fun' expression structure" << std::endl;
    exit(1);
  }

  std::string func_name = name_expr->get_name();
  std::vector<std::string> func_params;

  for (const auto& param : params_expr->get_exprs()) {
    if (auto param_symbol = std::dynamic_pointer_cast<symbol_expr>(param)) {
      func_params.push_back(param_symbol->get_name());
    } else {
      std::cerr << "error: 'fun' parameters must be symbols" << std::endl;
      exit(1);
    }
  }

  auto func =
      [params = std::move(func_params), body_expr = std::move(body_expr)](
          eval_context& ctx, std::vector<expr_value> args) -> expr_value {
    if (args.size() != params.size()) {
      std::cerr << "error: argument count does not match parameter count"
                << std::endl;
      exit(1);
    }

    eval_context& local_ctx = ctx;

    for (size_t i = 0; i < params.size(); ++i) {
      local_ctx.vmap[params[i]] = std::move(args[i]);
    }

    expr_value func_ret_value;

    for (const auto& expr : body_expr->get_exprs()) {
      func_ret_value = get_value_from_expr(local_ctx, expr);
    }

    return func_ret_value;
  };

  ctx.fmap.insert_or_assign(
      std::move(func_name),
      std::make_unique<callable_impl<decltype(func)>>(std::move(func)));

  return expr_value(
      std::make_unique<callable_impl<decltype(func)>>(std::move(func)));
}

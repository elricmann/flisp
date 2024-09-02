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

      switch (name[0]) {
        case '+':
          return eval_add(ctx, list_node);
          break;
        case '-':
          return eval_sub(ctx, list_node);
          break;
        case '*':
          return eval_mul(ctx, list_node);
          break;
        case '/':
          return eval_div(ctx, list_node);
          break;
        case 'i':
          if (name == "if") {
            return eval_if(ctx, list_node);
          }
          break;
        default:
          break;
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
        }
      }
    }
  }
}

// @todo: prevent redefinition & mutable-by-default
void interp::eval_def(eval_context& ctx,
                      const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  // auto value_expr =
  //     std::dynamic_pointer_cast<integer_expr>(lst->get_exprs()[2]);
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

#include "interp.h"

#include <iostream>

expr_value get_value_from_expr(
    const std::shared_ptr<expr>& node,
    const std::unordered_map<std::string, expr_value>& vmap) {
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

    if (vmap.find(name) != vmap.end()) {
      return vmap.at(name);
    } else {
      // return name; /*keep this for tests*/
      std::cerr << "error: identifier '" << name << "' not found" << std::endl;
      exit(1);
    }
  } else if (auto list_node = std::dynamic_pointer_cast<list_expr>(node)) {
    auto first_expr = list_node->get_exprs().front();
    auto symbol = std::dynamic_pointer_cast<symbol_expr>(first_expr);

    if (symbol) {
      const std::string& name = symbol->get_name();

      switch (name[0]) {
        case '+':
          return eval_add(list_node, vmap);
          break;
        case '-':
          return eval_sub(list_node, vmap);
          break;
        case '*':
          return eval_mul(list_node, vmap);
          break;
        case '/':
          return eval_div(list_node, vmap);
          break;
        default:
          break;
      }
    }
  }

  std::cerr << "error: unknown expression type" << std::endl;
  exit(1);
}

void interp::eval(const std::shared_ptr<expr>& node) {
  if (auto outer_lst = std::dynamic_pointer_cast<list_expr>(node)) {
    // we need this check to ensure that adjacent
    // nodes are not in conflict with nested nodes
    for (auto&& inner_lst : outer_lst->get_exprs()) {
      eval(inner_lst);
      skip_initial_lst = false;
    }

    skip_initial_lst = true;

    if (skip_initial_lst) {
      auto first_expr = outer_lst->get_exprs().front();
      auto symbol = std::dynamic_pointer_cast<symbol_expr>(first_expr);

      if (symbol) {
        const std::string& name = symbol->get_name();

        // clang-format off

        static const std::unordered_map<std::string, std::function<void(std::shared_ptr<list_expr>)>> dispatch_table = {
          {"def", [this](std::shared_ptr<list_expr> lst) { eval_def(lst); }},
          {"set", [this](std::shared_ptr<list_expr> lst) { eval_set(lst); }},
          {"debug", [this](std::shared_ptr<list_expr> lst) { eval_debug(lst); }}
        };

        // clang-format on

        auto it = dispatch_table.find(name);

        if (it != dispatch_table.end()) {
          it->second(outer_lst);
        }
      }
    }
  }
}

// @todo: prevent redefinition & mutable-by-default
void interp::eval_def(const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  // auto value_expr =
  //     std::dynamic_pointer_cast<integer_expr>(lst->get_exprs()[2]);
  auto value_expr = get_value_from_expr(lst->get_exprs()[2], vmap);

  if (symbol) {
    vmap[symbol->get_name()] = value_expr;
  }
}

void interp::eval_set(const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  auto value_expr = get_value_from_expr(lst->get_exprs()[2], vmap);

  if (symbol) {
    vmap[symbol->get_name()] = value_expr;
  }
}

void interp::eval_debug(const std::shared_ptr<list_expr>& list) {
  for (size_t i = 1; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(list->get_exprs()[i], vmap);

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

expr_value eval_add(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap) {
  float acc = 0;

  for (size_t i = 1; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(list->get_exprs()[i], vmap);

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

expr_value eval_sub(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap) {
  if (list->get_exprs().size() < 2) {
    std::cerr << "error: at least one operand required for sub" << std::endl;
    exit(1);
  }

  auto fst = get_value_from_expr(list->get_exprs()[1], vmap);
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
    auto value = get_value_from_expr(list->get_exprs()[i], vmap);

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

expr_value eval_mul(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap) {
  float acc = 1;

  for (size_t i = 1; i < list->get_exprs().size(); ++i) {
    auto value = get_value_from_expr(list->get_exprs()[i], vmap);

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

expr_value eval_div(const std::shared_ptr<list_expr>& list,
                    const std::unordered_map<std::string, expr_value>& vmap) {
  if (list->get_exprs().size() < 2) {
    std::cerr << "error: at least one operand required for div" << std::endl;
    exit(1);
  }

  auto fst = get_value_from_expr(list->get_exprs()[1], vmap);
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
    auto value = get_value_from_expr(list->get_exprs()[i], vmap);

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

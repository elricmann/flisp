#include "interp.h"

#include <iostream>

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

void interp::eval_def(const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  auto value_expr =
      std::dynamic_pointer_cast<integer_expr>(lst->get_exprs()[2]);

  if (symbol && value_expr) {
    variables[symbol->get_name()] = value_expr->get_value();
  }
}

void interp::eval_set(const std::shared_ptr<list_expr>& lst) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(lst->get_exprs()[1]);
  auto value_expr =
      std::dynamic_pointer_cast<integer_expr>(lst->get_exprs()[2]);

  if (symbol && value_expr) {
    variables[symbol->get_name()] = value_expr->get_value();
  }
}

void interp::eval_debug(const std::shared_ptr<list_expr>& list) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(list->get_exprs()[1]);

  if (symbol) {
    if (variables.find(symbol->get_name()) != variables.end()) {
      std::cout << variables[symbol->get_name()] << std::endl;
    } else {
      std::cerr << "error: variable " << symbol->get_name() << " not found"
                << std::endl;
    }
  }
}

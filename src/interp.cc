#include "interp.h"

#include <iostream>

void interp::eval(const std::shared_ptr<expr>& node) {
  if (auto list = std::dynamic_pointer_cast<list_expr>(node)) {
    auto first_expr = list->get_exprs().front();
    auto symbol = std::dynamic_pointer_cast<symbol_expr>(first_expr);

    // std::cout << list->get_exprs().size() << std::endl;
    if (symbol) {
      const std::string& name = symbol->get_name();

      if (name == "def") {
        eval_def(list);
      } else if (name == "set") {
        eval_set(list);
      } else if (name == "debug") {
        eval_debug(list);
      }
    }
  }
}

void interp::eval_def(const std::shared_ptr<list_expr>& list) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(list->get_exprs()[1]);
  auto value_expr =
      std::dynamic_pointer_cast<integer_expr>(list->get_exprs()[2]);

  if (symbol && value_expr) {
    variables[symbol->get_name()] = value_expr->get_value();
  }
}

void interp::eval_set(const std::shared_ptr<list_expr>& list) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(list->get_exprs()[1]);
  auto value_expr =
      std::dynamic_pointer_cast<integer_expr>(list->get_exprs()[2]);

  if (symbol && value_expr) {
    variables[symbol->get_name()] = value_expr->get_value();
  }
}

void interp::eval_debug(const std::shared_ptr<list_expr>& list) {
  auto symbol = std::dynamic_pointer_cast<symbol_expr>(list->get_exprs()[1]);

  if (symbol) {
    std::cout << variables[symbol->get_name()] << std::endl;
  }
}

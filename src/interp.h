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
  std::unordered_map<std::string, int> variables;

  void eval_def(const std::shared_ptr<list_expr>& list);
  void eval_set(const std::shared_ptr<list_expr>& list);
  void eval_debug(const std::shared_ptr<list_expr>& list);
};

#endif  // INTERP_H

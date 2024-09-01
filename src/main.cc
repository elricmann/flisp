#include <string>

#include "./lexer.h"
#include "./parser.h"

int main(int argc, char const* argv[]) {
  const std::string& source =
      "(def n 0)"
      "(def pi 3.14)"
      "(set n #t)"
      "(echo \"literal\")"
      "(+ m (/ m n))";
  std::vector<token> tokens = tokenize(source);

  std::cout << "---------------" << std::endl;
  std::cout << "     lexer     " << std::endl;
  std::cout << "---------------" << std::endl;

  std::cout << tokens.size() << std::endl;

  // for (auto&& token : tokens) {
  //   std::cout << token.get_type() << " → " << token.get_value() << std::endl;
  // }

  std::cout << "----------------" << std::endl;
  std::cout << "     parser     " << std::endl;
  std::cout << "----------------" << std::endl;

  std::shared_ptr<expr> expr_tree = parser(tokens).parse();

  std::unordered_map<std::type_index,
                     std::function<void(std::shared_ptr<expr>)>>
      callbacks;

  callbacks[typeid(symbol_expr)] = [](std::shared_ptr<expr> node) {
    auto sym_node = std::dynamic_pointer_cast<symbol_expr>(node);
    std::cout << "found symbol: " << sym_node->get_name() << std::endl;
  };

  callbacks[typeid(integer_expr)] = [](std::shared_ptr<expr> node) {
    auto integer_literal_node = std::dynamic_pointer_cast<integer_expr>(node);
    std::cout << "found integer: " << integer_literal_node->get_value()
              << std::endl;
  };

  callbacks[typeid(float_expr)] = [](std::shared_ptr<expr> node) {
    auto float_literal_node = std::dynamic_pointer_cast<float_expr>(node);
    std::cout << "found float: " << float_literal_node->get_value()
              << std::endl;
  };

  callbacks[typeid(list_expr)] = [](std::shared_ptr<expr> node) {
    auto list_node = std::dynamic_pointer_cast<list_expr>(node);
    std::cout << "entering list (size): " << list_node->get_exprs().size()
              << std::endl;
  };

  visit(expr_tree, callbacks);

  return 0;
}

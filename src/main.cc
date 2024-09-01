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

  callbacks[typeid(number_expr)] = [](std::shared_ptr<expr> node) {
    auto num_node = std::dynamic_pointer_cast<number_expr>(node);
    std::cout << "found number: " << num_node->get_value() << std::endl;
  };

  visit(expr_tree, callbacks);

  return 0;
}

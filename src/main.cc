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

  auto expr_ast = parser(tokens).parse();

  return 0;
}

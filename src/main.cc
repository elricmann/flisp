#include <string>

#include "./lexer.h"

int main(int argc, char const* argv[]) {
  const std::string& source = " (def n 3.14)";
  std::vector<token> tokens = tokenize(source);

  std::cout << tokens.size() << std::endl;

  for (auto&& token : tokens) {
    std::cout << token.get_type() << " → " << token.get_value() << std::endl;
  }

  return 0;
}

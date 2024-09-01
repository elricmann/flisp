#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#include "./emit.h"
#include "./lexer.h"
#include "./parser.h"

void compile(const std::string& source);

std::string read_file(const std::string& file_path) {
  std::ifstream file(file_path);

  if (!file) {
    throw std::runtime_error("file not found: " + file_path);
  }

  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

void argparse(int argc, char const* argv[]) {
  std::unordered_map<std::string, std::function<void(const std::string&)>>
      actions = {{"-c", [](const std::string& file_path) {
                    compile(read_file(file_path));
                  }}};

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (actions.find(arg) != actions.end() && i + 1 < argc) {
      actions[arg](argv[++i]);
    }
  }
}

void compile(const std::string& source) {
  std::vector<token> tokens = tokenize(source);

  // std::cout << "---------------" << std::endl;
  // std::cout << "     lexer     " << std::endl;
  // std::cout << "---------------" << std::endl;

  // std::cout << tokens.size() << std::endl;

  // for (auto&& token : tokens) {
  //   std::cout << token.get_type() << " → " << token.get_value() << std::endl;
  // }

  // std::cout << "----------------" << std::endl;
  // std::cout << "     parser     " << std::endl;
  // std::cout << "----------------" << std::endl;

  std::shared_ptr<expr> expr_tree = parser(tokens).parse();

  __test_emit__();

  // std::unordered_map<std::type_index,
  //                    std::function<void(std::shared_ptr<expr>)>>
  //     callbacks;

  // callbacks[typeid(symbol_expr)] = [](std::shared_ptr<expr> node) {
  //   auto sym_node = std::dynamic_pointer_cast<symbol_expr>(node);
  //   std::cout << "found symbol: " << sym_node->get_name() << std::endl;
  // };

  // callbacks[typeid(integer_expr)] = [](std::shared_ptr<expr> node) {
  //   auto integer_literal_node =
  //   std::dynamic_pointer_cast<integer_expr>(node); std::cout << "found
  //   integer: " << integer_literal_node->get_value()
  //             << std::endl;
  // };

  // callbacks[typeid(float_expr)] = [](std::shared_ptr<expr> node) {
  //   auto float_literal_node = std::dynamic_pointer_cast<float_expr>(node);
  //   std::cout << "found float: " << float_literal_node->get_value()
  //             << std::endl;
  // };

  // callbacks[typeid(list_expr)] = [](std::shared_ptr<expr> node) {
  //   auto list_node = std::dynamic_pointer_cast<list_expr>(node);
  //   std::cout << "entering list (size): " << list_node->get_exprs().size()
  //             << std::endl;
  // };

  // visit(expr_tree, callbacks);
}

int main(int argc, char const* argv[]) {
  argparse(argc, argv);
  return 0;
}

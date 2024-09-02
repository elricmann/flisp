#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#include "./emit.h"
#include "./interp.h"
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
  const std::shared_ptr<expr>& expr_tree = parser(tokens).parse();
  eval_context ctx;
  interp().eval(ctx, expr_tree);
}

int main(int argc, char const* argv[]) {
  argparse(argc, argv);
  return 0;
}

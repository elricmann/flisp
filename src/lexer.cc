#include "./lexer.h"

std::vector<token> tokenize(const std::string& source) {
  lexer lex(source);
  std::vector<token> tokens;

  token tok = lex.next_token();

  while (tok.get_type() != token_type::token_end_of_file) {
    tokens.push_back(tok);
    tok = lex.next_token();
  }

  // @todo: fix this insertion issue
  tokens.insert(tokens.begin(), token{token_type::token_left_paren, "("});

  return tokens;
}

token_type token::get_type() const { return type_; }

std::string token::get_value() const { return value_; }

lexer::lexer(const std::string& source) : source_(source), current_pos_(0) {
  eat();
}

token lexer::next_token() {
  skip_whitespace();

  if (current_pos_ >= source_.size()) {
    return token(token_type::token_end_of_file, "");
  }

  char current = current_char();

  if (current == '(') {
    eat();
    return token(token_type::token_left_paren, "(");
  } else if (current == ')') {
    eat();
    return token(token_type::token_right_paren, ")");
  } else if (std::isdigit(current) ||
             (current == '.' && std::isdigit(peek_char()))) {
    return number();
  } else if (std::isalpha(current) || current == '_' || current_char() == '_' ||
             current_char() == '+' || current_char() == '-' ||
             current_char() == '*' || current_char() == '/' ||
             current_char() == '=') {
    return symbol();
  } else if (current == '#' && (peek_char() == 't' || peek_char() == 'f')) {
    return boolean();
  } else if (current == '"') {
    return string_literal();
  } else {
    throw std::runtime_error("Unexpected character: " +
                             std::string(1, current));
  }
}

token lexer::number() {
  std::string value;
  bool is_float = false;

  while (current_pos_ < source_.size() &&
         (std::isdigit(current_char()) || current_char() == '.')) {
    if (current_char() == '.') {
      if (is_float) {  // ensure there's only one decimal point
        throw std::runtime_error("Multiple decimal points found in a number.");
      }

      is_float = true;
    }

    value += current_char();

    eat();
  }

  if (value == "." || (value.find('.') == 0 && value.length() == 1)) {
    throw std::runtime_error("Invalid numeric format.");
  }

  return is_float ? token(token_type::token_float, value)
                  : token(token_type::token_integer, value);
}

token lexer::boolean() {
  std::string value;

  eat();  // skip the '#'

  if (current_char() == 't') {
    value = "#t";
  } else if (current_char() == 'f') {
    value = "#f";
  } else {
    throw std::runtime_error("unexpected boolean value");
  }

  eat();  // skip the 't' or 'f'

  return token(token_type::token_boolean, value);
}

token lexer::string_literal() {
  std::string value;
  eat();  // skip the opening '"'

  while (current_pos_ < source_.size() && current_char() != '"') {
    value += current_char();
    eat();
  }

  if (current_char() == '"') {
    eat();  // skip the closing '"'
  } else {
    throw std::runtime_error("unclosed string literal");
  }

  return token(token_type::token_string_literal, value);
}

char lexer::current_char() const { return source_[current_pos_]; }

char lexer::peek_char() const {
  if (current_pos_ + 1 < source_.size()) {
    return source_[current_pos_ + 1];
  }

  return '\0';  // return null character if EOF
}

void lexer::eat() {
  if (current_pos_ < source_.size()) {
    current_pos_++;
  }
}

void lexer::skip_whitespace() {
  while (current_pos_ < source_.size() && std::isspace(current_char())) {
    eat();
  }
}

// token lexer::number() {
//   std::size_t start_pos = current_pos_;

//   while (current_pos_ < source_.size() && std::isdigit(current_char())) {
//     eat();
//   }

//   return token(token_type::token_number,
//                source_.substr(start_pos, current_pos_ - start_pos));
// }

token lexer::symbol() {
  std::size_t start_pos = current_pos_;
  std::string value;

  while (current_pos_ < source_.size() &&
         (std::isalnum(current_char()) || current_char() == '_' ||
          current_char() == '+' || current_char() == '-' ||
          current_char() == '*' || current_char() == '/' ||
          current_char() == '=')) {
    value += current_char();
    eat();
  }

  while (current_pos_ < source_.size() &&
         (std::isalnum(current_char()) || current_char() == '_')) {
    eat();
  }

  return value.length()
             ? token(token_type::token_symbol, value)
             : token(token_type::token_symbol,
                     source_.substr(start_pos, current_pos_ - start_pos));
}

std::ostream& operator<<(std::ostream& os, token_type type) {
  switch (type) {
    case token_type::token_integer:
      return os << "integer";
    case token_type::token_float:
      return os << "float";
    case token_type::token_boolean:
      return os << "boolean";
    case token_type::token_string_literal:
      return os << "string_literal";
    case token_type::token_symbol:
      return os << "symbol";
    case token_type::token_left_paren:
      return os << "left_paren";
    case token_type::token_right_paren:
      return os << "right_paren";
    case token_type::token_end_of_file:
      return os << "end_of_file";
  }

  return os << "unknown";
}

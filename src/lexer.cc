#include "./lexer.h"

token_type token::get_type() const { return type_; }

std::string token::get_value() const { return value_; }

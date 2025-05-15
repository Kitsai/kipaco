#pragma once

#include "Parser.h"

namespace kipaco::char_parser {

Parser<char> any_char();

Parser<char> chosen_char(char c);

Parser<char> digit();

Parser<char> alphabetic();

Parser<char> alphanumeric();

Parser<char> word_char();
} // namespace kipaco::char_parser

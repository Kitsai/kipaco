#pragma once

#include "Parser.h"

namespace kipaco::char_parser {

Parser<char> any_char();

Parser<char> chosen_char(char c);
} // namespace kipaco::char_parser

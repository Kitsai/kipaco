#pragma once

#include "Parser.h"
#include <variant>

namespace kipaco {
Parser<std::monostate> literal(const std::string& expected);

} // namespace kipaco

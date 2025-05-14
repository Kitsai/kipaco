#pragma once

#include <stdexcept>
namespace kipaco {
struct parse_error : public std::runtime_error {
    parse_error(const std::string& e) : std::runtime_error("[PARSING ERROR] - " + e) {}
};

} // namespace kipaco

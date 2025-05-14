#include "basic_parsers/pure.h"
#include "Parser.h"

namespace kipaco {
Parser<std::string> pure() {
    return {[](const std::string& input) -> Parser<std::string>::ParseResult {
        return Parser<std::string>::ParseResultType{"", input};
    }};
}
} // namespace kipaco

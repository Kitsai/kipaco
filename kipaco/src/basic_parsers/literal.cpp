#include "basic_parsers/literal.h"
#include "Parser.h"
#include <variant>

namespace kipaco {

Parser<std::monostate> literal(const std::string& expected) {
    return {[expected](const std::string& input) -> Parser<std::monostate>::ParseResult {
        bool res = input.rfind(expected, 0) == 0;
        if(res)
            return typename Parser<std::monostate>::ParseResultType{input.substr(expected.size()), {}};

        return cpp::fail(input);
    }};
}

} // namespace kipaco

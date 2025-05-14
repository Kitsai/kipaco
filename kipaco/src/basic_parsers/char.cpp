#include "basic_parsers/char.h"
#include "Parser.h"
#include "result.hpp"

namespace kipaco::char_parser {

using ParseResultType = Parser<char>::ParseResultType;

Parser<char> any_char() {
    return {[](const std::string& s) -> Parser<char>::ParseResult {
        if(s.empty())
            return cpp::fail(s);

        return ParseResultType{s.substr(1), s[0]};
    }};
}

Parser<char> chosen_char(char expected) {
    return any_char().pred([expected](const char& c) { return c == expected; });
}

} // namespace kipaco::char_parser

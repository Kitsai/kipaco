#include "basic_parsers/char.h"
#include "Parser.h"
#include "combinators/either.h"

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

Parser<char> digit() {
    return any_char().pred([](const char& c) { return '0' <= c && c <= '9'; });
}

Parser<char> alphabetic() {
    return any_char().pred([](const char& c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'); });
}

Parser<char> alphanumeric() { return alphabetic() | digit(); }

Parser<char> word_char() { return alphanumeric() | chosen_char('_'); }

} // namespace kipaco::char_parser

#pragma once

#include "Parser.h"
#include "basic_parsers/literal.h"
#include <cstdlib>
#include <variant>
namespace kipaco {

template <typename T> Parser<T> either(const Parser<T>& p1, const Parser<T>& p2) {
    return {[p1, p2](const std::string& input) -> typename Parser<T>::ParseResult {
        auto res1 = p1(input);

        if(res1.has_value())
            return res1;

        return p2(input);
    }};
}

template <typename T> inline Parser<T> operator|(const Parser<T>& p1, const Parser<T>& p2) { return either(p1, p2); }

inline Parser<std::monostate> operator|(const Parser<std::monostate>& p, const std::string& s) {
    return either(p, literal(s));
}

inline Parser<std::monostate> operator|(const std::string& s, const Parser<std::monostate>& p) {
    return either(literal(s), p);
}
} // namespace kipaco
